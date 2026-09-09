#pragma once
#include <cstdint>
namespace sensor {
enum class SensorStatus : std::uint8_t { Ok, DriverError, NotReady };
struct SensorReading { const char* sensor_id; float value_celsius; std::uint32_t timestamp_ms; SensorStatus status; };
}  // namespace sensor
