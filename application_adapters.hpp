#pragma once
#include "sensor/event_bus.hpp"
namespace sensor {
class IGuiTemperatureView { public: virtual ~IGuiTemperatureView() = default; virtual void show_temperature(const char* sensor_id, float celsius, bool valid) = 0; };
class GuiModel final : public ISensorSubscriber { public: explicit GuiModel(IGuiTemperatureView& view) : view_(view) {} void on_reading(const SensorReading& reading) override; private: IGuiTemperatureView& view_; };
class IMqttClient { public: virtual ~IMqttClient() = default; virtual bool publish(const char* topic, const char* payload) = 0; };
class MqttPublisher final : public ISensorSubscriber { public: MqttPublisher(IMqttClient& client, const char* topic_prefix); void on_reading(const SensorReading& reading) override; private: IMqttClient& client_; const char* topic_prefix_; };
class ITemperatureBusinessRule { public: virtual ~ITemperatureBusinessRule() = default; virtual void evaluate(const SensorReading& reading) = 0; };
class BusinessCalculator final : public ISensorSubscriber { public: explicit BusinessCalculator(ITemperatureBusinessRule& rule) : rule_(rule) {} void on_reading(const SensorReading& reading) override; private: ITemperatureBusinessRule& rule_; };
}  // namespace sensor
