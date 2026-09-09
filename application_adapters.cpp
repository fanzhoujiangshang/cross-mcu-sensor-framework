#include "sensor/application_adapters.hpp"
#include <cstdio>
namespace sensor {
void GuiModel::on_reading(const SensorReading& reading) { view_.show_temperature(reading.sensor_id, reading.value_celsius, reading.status == SensorStatus::Ok); }
MqttPublisher::MqttPublisher(IMqttClient& client, const char* topic_prefix) : client_(client), topic_prefix_(topic_prefix) {}
void MqttPublisher::on_reading(const SensorReading& reading) { char topic[128]; char payload[160]; std::snprintf(topic, sizeof(topic), "%s/%s/temperature", topic_prefix_, reading.sensor_id); std::snprintf(payload, sizeof(payload), "{\"value_celsius\":%.2f,\"timestamp_ms\":%u,\"status\":%u}", static_cast<double>(reading.value_celsius), reading.timestamp_ms, static_cast<unsigned>(reading.status)); client_.publish(topic, payload); }
void BusinessCalculator::on_reading(const SensorReading& reading) { rule_.evaluate(reading); }
}  // namespace sensor
