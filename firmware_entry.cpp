#include "sensor_framework.hpp"
class BoardTemperatureDriver final : public sensor::ITemperatureDriver { public: bool read_celsius(float& value) override { (void)value; return false; } };
class BoardClock final : public sensor::IClock { public: std::uint32_t milliseconds() const override { return ticks_; } void advance(std::uint32_t elapsed) { ticks_ += elapsed; } private: std::uint32_t ticks_{0}; };
class NoopGui final : public sensor::IGuiTemperatureView { public: void show_temperature(const char*, float, bool) override {} };
class NoopMqtt final : public sensor::IMqttClient { public: bool publish(const char*, const char*) override { return true; } };
class NoopBusiness final : public sensor::ITemperatureBusinessRule { public: void evaluate(const sensor::SensorReading&) override {} };
static void application_loop() { BoardTemperatureDriver driver; BoardClock clock; float history[5]{}, scratch[5]{}; sensor::MedianFilter filter(history, scratch, 5); sensor::SensorDispatcher bus; NoopGui view; NoopMqtt client; NoopBusiness rule; sensor::GuiModel gui(view); sensor::MqttPublisher mqtt(client, "sensors"); sensor::BusinessCalculator business(rule); bus.subscribe(gui); bus.subscribe(mqtt); bus.subscribe(business); sensor::TemperatureSensor temperature("board-temperature", driver, filter, clock, bus); for (;;) { temperature.sample(); clock.advance(1000); } }
#if defined(ESP_PLATFORM)
extern "C" void app_main() { application_loop(); }
#else
int main() { application_loop(); }
#endif
