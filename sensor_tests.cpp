#include "sensor/filter.hpp"
#include "sensor/temperature_sensor.hpp"
#include <cassert>
#include <cmath>
namespace { bool near(float a, float b) { return std::fabs(a - b) < 0.001F; }
class Driver final : public sensor::ITemperatureDriver { public: bool read_celsius(float& v) override { v = 25.0F; return true; } };
class Clock final : public sensor::IClock { public: std::uint32_t milliseconds() const override { return 42; } };
class Capture final : public sensor::ISensorSubscriber { public: void on_reading(const sensor::SensorReading& r) override { reading = r; called = true; } sensor::SensorReading reading{}; bool called{false}; }; }
int main() { float storage[3]{}; sensor::MovingAverageFilter avg(storage, 3); assert(near(avg.apply(1), 1)); assert(near(avg.apply(2), 1.5F)); assert(near(avg.apply(6), 3)); assert(near(avg.apply(9), 17.0F / 3.0F)); sensor::ExponentialMovingAverageFilter ema(0.5F); assert(near(ema.apply(10), 10)); assert(near(ema.apply(14), 12)); Driver driver; Clock clock; sensor::PassthroughFilter pass; sensor::SensorEventBus bus; Capture capture; bus.subscribe(capture); sensor::TemperatureSensor temperature("test", driver, pass, clock, bus); const auto r = temperature.sample(); assert(capture.called && r.status == sensor::SensorStatus::Ok && near(capture.reading.value_celsius, 25)); }
