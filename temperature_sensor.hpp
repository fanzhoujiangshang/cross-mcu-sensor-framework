#pragma once
#include "sensor/event_bus.hpp"
#include "sensor/filter.hpp"
namespace sensor {
class ITemperatureDriver { public: virtual ~ITemperatureDriver() = default; virtual bool read_celsius(float& value) = 0; };
class IClock { public: virtual ~IClock() = default; virtual std::uint32_t milliseconds() const = 0; };
class TemperatureSensor {
public: TemperatureSensor(const char* id, ITemperatureDriver& driver, IFilter& filter, const IClock& clock, SensorEventBus& event_bus); SensorReading sample();
private: const char* id_; ITemperatureDriver& driver_; IFilter& filter_; const IClock& clock_; SensorEventBus& event_bus_;
};
}  // namespace sensor
