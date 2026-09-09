#include "sensor/filter.hpp"
#include <algorithm>
#include <stdexcept>
namespace sensor {
MovingAverageFilter::MovingAverageFilter(float* storage, std::size_t capacity) : storage_(storage), capacity_(capacity) { if (!storage || !capacity) throw std::invalid_argument("moving average needs storage"); }
float MovingAverageFilter::apply(float sample) { if (count_ == capacity_) sum_ -= storage_[next_]; else ++count_; storage_[next_] = sample; sum_ += sample; next_ = (next_ + 1) % capacity_; return sum_ / static_cast<float>(count_); }
void MovingAverageFilter::reset() { count_ = 0; next_ = 0; sum_ = 0.0F; }
ExponentialMovingAverageFilter::ExponentialMovingAverageFilter(float alpha) : alpha_(alpha) { if (alpha <= 0.0F || alpha > 1.0F) throw std::invalid_argument("alpha must be in (0, 1]"); }
float ExponentialMovingAverageFilter::apply(float sample) { if (!initialized_) { initialized_ = true; value_ = sample; } else value_ += alpha_ * (sample - value_); return value_; }
void ExponentialMovingAverageFilter::reset() { initialized_ = false; value_ = 0.0F; }
MedianFilter::MedianFilter(float* storage, float* work_buffer, std::size_t capacity) : storage_(storage), work_buffer_(work_buffer), capacity_(capacity) { if (!storage || !work_buffer || !capacity) throw std::invalid_argument("median needs two buffers"); }
float MedianFilter::apply(float sample) { storage_[next_] = sample; next_ = (next_ + 1) % capacity_; if (count_ < capacity_) ++count_; for (std::size_t i = 0; i < count_; ++i) work_buffer_[i] = storage_[i]; std::sort(work_buffer_, work_buffer_ + count_); const auto middle = count_ / 2; return count_ % 2 ? work_buffer_[middle] : (work_buffer_[middle - 1] + work_buffer_[middle]) / 2.0F; }
void MedianFilter::reset() { count_ = 0; next_ = 0; }
}  // namespace sensor
