#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <vector>

namespace sensor {

enum class SensorStatus : std::uint8_t { Ok, DriverError, NotReady };
struct SensorReading { const char* sensor_id; float value_celsius; std::uint32_t timestamp_ms; SensorStatus status; };

// HAL boundary: provide an MCU-specific implementation for STM32, ESP32, RP2040, etc.
class ITemperatureDriver { public: virtual ~ITemperatureDriver() = default; virtual bool read_celsius(float& value) = 0; };
class IClock { public: virtual ~IClock() = default; virtual std::uint32_t milliseconds() const = 0; };

class IFilter { public: virtual ~IFilter() = default; virtual float apply(float sample) = 0; virtual void reset() = 0; };
class PassthroughFilter final : public IFilter { public: float apply(float s) override { return s; } void reset() override {} };
class MovingAverageFilter final : public IFilter {
public: MovingAverageFilter(float* buffer, std::size_t size) : b_(buffer), n_(size) { if (!b_ || !n_) throw std::invalid_argument("buffer required"); }
 float apply(float s) override { if (count_ == n_) sum_ -= b_[next_]; else ++count_; b_[next_] = s; sum_ += s; next_ = (next_ + 1) % n_; return sum_ / count_; } void reset() override { count_ = next_ = 0; sum_ = 0; }
private: float* b_; std::size_t n_, count_{0}, next_{0}; float sum_{0}; };
class ExponentialMovingAverageFilter final : public IFilter {
public: explicit ExponentialMovingAverageFilter(float alpha) : a_(alpha) { if (a_ <= 0 || a_ > 1) throw std::invalid_argument("alpha in (0,1]"); } float apply(float s) override { if (!ready_) { ready_ = true; v_ = s; } else v_ += a_ * (s - v_); return v_; } void reset() override { ready_ = false; v_ = 0; }
private: float a_; bool ready_{false}; float v_{0}; };
class MedianFilter final : public IFilter {
public: MedianFilter(float* history, float* scratch, std::size_t size) : h_(history), s_(scratch), n_(size) { if (!h_ || !s_ || !n_) throw std::invalid_argument("two buffers required"); } float apply(float v) override { h_[next_] = v; next_ = (next_ + 1) % n_; if (count_ < n_) ++count_; for (std::size_t i = 0; i < count_; ++i) s_[i] = h_[i]; std::sort(s_, s_ + count_); const auto m = count_ / 2; return count_ % 2 ? s_[m] : (s_[m - 1] + s_[m]) / 2; } void reset() override { count_ = next_ = 0; }
private: float* h_; float* s_; std::size_t n_, count_{0}, next_{0}; };
class KalmanFilter final : public IFilter {
public: KalmanFilter(float process_noise, float measurement_noise, float initial_error = 1) : q_(process_noise), r_(measurement_noise), e0_(initial_error), e_(initial_error) { if (q_ < 0 || r_ <= 0 || e0_ <= 0) throw std::invalid_argument("invalid noise"); } float apply(float sample) override { if (!ready_) { ready_ = true; x_ = sample; return x_; } e_ += q_; const float k = e_ / (e_ + r_); x_ += k * (sample - x_); e_ *= 1 - k; return x_; } void reset() override { ready_ = false; x_ = 0; e_ = e0_; }
private: float q_, r_, e0_, e_; bool ready_{false}; float x_{0}; };

class ISensorSubscriber { public: virtual ~ISensorSubscriber() = default; virtual void on_reading(const SensorReading& reading) = 0; };
class SensorDispatcher { public: void subscribe(ISensorSubscriber& s) { if (std::find(items_.begin(), items_.end(), &s) == items_.end()) items_.push_back(&s); } void publish(const SensorReading& r) const { for (auto* s : items_) s->on_reading(r); } private: std::vector<ISensorSubscriber*> items_; };

class SensorBase {
public: virtual ~SensorBase() = default; virtual SensorReading sample() = 0;
protected: SensorBase(const char* id, IFilter& filter, const IClock& clock, SensorDispatcher& dispatcher) : id_(id), filter_(filter), clock_(clock), dispatcher_(dispatcher) {} SensorReading publish_raw(float raw, bool valid) { SensorReading r{id_, valid ? filter_.apply(raw) : 0.0F, clock_.milliseconds(), valid ? SensorStatus::Ok : SensorStatus::DriverError}; dispatcher_.publish(r); return r; }
private: const char* id_; IFilter& filter_; const IClock& clock_; SensorDispatcher& dispatcher_; };
class TemperatureSensor final : public SensorBase { public: TemperatureSensor(const char* id, ITemperatureDriver& driver, IFilter& filter, const IClock& clock, SensorDispatcher& dispatcher) : SensorBase(id, filter, clock, dispatcher), driver_(driver) {} SensorReading sample() override { float value = 0; return publish_raw(value, driver_.read_celsius(value)); } private: ITemperatureDriver& driver_; };

class IGuiTemperatureView { public: virtual ~IGuiTemperatureView() = default; virtual void show_temperature(const char* id, float celsius, bool valid) = 0; };
class GuiModel final : public ISensorSubscriber { public: explicit GuiModel(IGuiTemperatureView& view) : view_(view) {} void on_reading(const SensorReading& r) override { view_.show_temperature(r.sensor_id, r.value_celsius, r.status == SensorStatus::Ok); } private: IGuiTemperatureView& view_; };
class IMqttClient { public: virtual ~IMqttClient() = default; virtual bool publish(const char* topic, const char* payload) = 0; };
class MqttPublisher final : public ISensorSubscriber { public: MqttPublisher(IMqttClient& client, const char* prefix) : client_(client), prefix_(prefix) {} void on_reading(const SensorReading& r) override { char topic[128], json[160]; std::snprintf(topic, sizeof(topic), "%s/%s/temperature", prefix_, r.sensor_id); std::snprintf(json, sizeof(json), "{\"value_celsius\":%.2f,\"timestamp_ms\":%u,\"status\":%u}", static_cast<double>(r.value_celsius), r.timestamp_ms, static_cast<unsigned>(r.status)); client_.publish(topic, json); } private: IMqttClient& client_; const char* prefix_; };
class ITemperatureBusinessRule { public: virtual ~ITemperatureBusinessRule() = default; virtual void evaluate(const SensorReading& r) = 0; };
class BusinessCalculator final : public ISensorSubscriber { public: explicit BusinessCalculator(ITemperatureBusinessRule& rule) : rule_(rule) {} void on_reading(const SensorReading& r) override { rule_.evaluate(r); } private: ITemperatureBusinessRule& rule_; };
}  // namespace sensor
