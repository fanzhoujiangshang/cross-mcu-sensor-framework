#pragma once
#include <cstddef>
namespace sensor {
class IFilter { public: virtual ~IFilter() = default; virtual float apply(float sample) = 0; virtual void reset() = 0; };
class PassthroughFilter final : public IFilter { public: float apply(float sample) override { return sample; } void reset() override {} };
class MovingAverageFilter final : public IFilter {
public: MovingAverageFilter(float* storage, std::size_t capacity); float apply(float sample) override; void reset() override;
private: float* storage_; std::size_t capacity_; std::size_t count_{0}; std::size_t next_{0}; float sum_{0.0F}; };
class ExponentialMovingAverageFilter final : public IFilter {
public: explicit ExponentialMovingAverageFilter(float alpha); float apply(float sample) override; void reset() override;
private: float alpha_; bool initialized_{false}; float value_{0.0F}; };
class MedianFilter final : public IFilter {
public: MedianFilter(float* storage, float* work_buffer, std::size_t capacity); float apply(float sample) override; void reset() override;
private: float* storage_; float* work_buffer_; std::size_t capacity_; std::size_t count_{0}; std::size_t next_{0}; };
}  // namespace sensor
