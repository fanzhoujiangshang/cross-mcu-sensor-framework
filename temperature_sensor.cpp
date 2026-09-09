#include "sensor/temperature_sensor.hpp"
namespace sensor {
TemperatureSensor::TemperatureSensor(const char* id, ITemperatureDriver& driver, IFilter& filter, const IClock& clock, SensorEventBus& event_bus) : id_(id), driver_(driver), filter_(filter), clock_(clock), event_bus_(event_bus) {}
SensorReading TemperatureSensor::sample() { float raw = 0.0F; const bool valid = driver_.read_celsius(raw); SensorReading reading{id_, valid ? filter_.apply(raw) : 0.0F, clock_.milliseconds(), valid ? SensorStatus::Ok : SensorStatus::DriverError}; event_bus_.publish(reading); return reading; }
}  // namespace sensor
