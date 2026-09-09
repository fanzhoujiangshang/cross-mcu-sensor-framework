#include "sensor_framework.hpp"
#include <cstdio>
class Driver final : public sensor::ITemperatureDriver { public: bool read_celsius(float& v) override { v = values_[i_++ % 5]; return true; } private: float values_[5]{22, 22.2F, 34, 22.1F, 22}; std::size_t i_{0}; };
class Clock final : public sensor::IClock { public: std::uint32_t milliseconds() const override { return tick_++ * 1000; } private: mutable std::uint32_t tick_{0}; };
class Gui final : public sensor::IGuiTemperatureView { public: void show_temperature(const char*, float v, bool ok) override { std::printf("GUI %.2f C (%s)\n", v, ok ? "ok" : "error"); } };
class Mqtt final : public sensor::IMqttClient { public: bool publish(const char* t, const char* p) override { std::printf("MQTT %s %s\n", t, p); return true; } };
class Rule final : public sensor::ITemperatureBusinessRule { public: void evaluate(const sensor::SensorReading& r) override { if (r.value_celsius > 30) std::puts("Business: high-temperature alarm"); } };
int main() { Driver d; Clock c; float history[3]{}, scratch[3]{}; sensor::MedianFilter f(history, scratch, 3); sensor::SensorDispatcher bus; Gui gui_view; Mqtt mqtt; Rule rule; sensor::GuiModel gui(gui_view); sensor::MqttPublisher pub(mqtt, "plant/sensors"); sensor::BusinessCalculator business(rule); bus.subscribe(gui); bus.subscribe(pub); bus.subscribe(business); sensor::TemperatureSensor temp("tank-01", d, f, c, bus); for (int i = 0; i < 5; ++i) temp.sample(); }
