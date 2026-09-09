#include "sensor/application_adapters.hpp"
#include "sensor/temperature_sensor.hpp"
#include <cstdio>
class FakeDriver final : public sensor::ITemperatureDriver { public: bool read_celsius(float& out) override { out = values[index++ % 5]; return true; } private: float values[5]{22.0F, 22.2F, 34.0F, 22.1F, 22.0F}; std::size_t index{0}; };
class Clock final : public sensor::IClock { public: std::uint32_t milliseconds() const override { return tick++ * 1000; } private: mutable std::uint32_t tick{0}; };
class ConsoleGui final : public sensor::IGuiTemperatureView { public: void show_temperature(const char*, float c, bool ok) override { std::printf("GUI: %.2f C (%s)\n", c, ok ? "ok" : "error"); } };
class ConsoleMqtt final : public sensor::IMqttClient { public: bool publish(const char* t, const char* p) override { std::printf("MQTT %s %s\n", t, p); return true; } };
class Alarm final : public sensor::ITemperatureBusinessRule { public: void evaluate(const sensor::SensorReading& r) override { if (r.status == sensor::SensorStatus::Ok && r.value_celsius > 30.0F) std::puts("business: over-temperature"); } };
int main() {
 FakeDriver driver; Clock clock; float history[3]{}; float work[3]{}; sensor::MedianFilter filter(history, work, 3); sensor::SensorEventBus bus;
 ConsoleGui view; ConsoleMqtt mqtt; Alarm alarm; sensor::GuiModel gui(view); sensor::MqttPublisher publisher(mqtt, "plant/sensors"); sensor::BusinessCalculator business(alarm);
 bus.subscribe(gui); bus.subscribe(publisher); bus.subscribe(business); sensor::TemperatureSensor temperature("tank-01", driver, filter, clock, bus);
 for (int i = 0; i < 5; ++i) temperature.sample();
}
