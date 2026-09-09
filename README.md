# Cross-MCU Sensor Framework

A small, dependency-free C++17 framework for projects that need the same sensor
application code on several MCU families. The core is independent of the RTOS,
network stack, GUI toolkit, and hardware SDK.

## Design

```
MCU temperature driver -> TemperatureSensor -> Filter -> SensorEventBus
                                                   |-> GUI subscriber
                                                   |-> MQTT subscriber
                                                   `-> business subscriber
```

* `ITemperatureDriver` is the only hardware boundary. Implement it for an ADC,
  I2C device, HAL, Arduino library, or host test fake.
* `IFilter` lets each sensor select `none`, moving average, exponential moving
  average, or median filtering at construction time.
* `SensorEventBus` publishes immutable readings to independent subscribers;
  the application layer does not depend on sensor drivers.
* The core uses only the C++17 standard library and has no heap allocation in
  its sampling path (apart from subscriber setup).

## Build and run the host demonstration

```sh
cmake -S . -B build
cmake --build build
./build/sensor_demo
```

For an MCU, add the files under `include/` and `src/` to the firmware project,
provide an `ITemperatureDriver`, and call `TemperatureSensor::sample()` from a
timer or task.

## Extending it

Add a new filter by implementing `sensor::IFilter`. Add another sensor type by
following `TemperatureSensor`: read through a narrow driver interface, filter
the value, stamp it with the platform clock, and publish a `SensorReading`.

`MqttPublisher` and `GuiModel` deliberately define small adapter interfaces,
so an ESP-IDF, STM32Cube, Qt, LVGL, or other application can connect without
pulling those dependencies into the core.
