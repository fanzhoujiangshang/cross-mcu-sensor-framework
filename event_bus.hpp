#pragma once
#include "sensor/types.hpp"
#include <algorithm>
#include <vector>
namespace sensor {
class ISensorSubscriber { public: virtual ~ISensorSubscriber() = default; virtual void on_reading(const SensorReading& reading) = 0; };
// Subscribe during startup, before sampling begins, for a simple deterministic MCU loop.
class SensorEventBus {
public:
 void subscribe(ISensorSubscriber& subscriber) { if (std::find(subscribers_.begin(), subscribers_.end(), &subscriber) == subscribers_.end()) subscribers_.push_back(&subscriber); }
 void unsubscribe(ISensorSubscriber& subscriber) { subscribers_.erase(std::remove(subscribers_.begin(), subscribers_.end(), &subscriber), subscribers_.end()); }
 void publish(const SensorReading& reading) const { for (const auto* subscriber : subscribers_) subscriber->on_reading(reading); }
private: std::vector<ISensorSubscriber*> subscribers_;
};
}  // namespace sensor
