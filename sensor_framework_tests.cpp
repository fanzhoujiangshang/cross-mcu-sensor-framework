#include "sensor_framework.hpp"
#include <cassert>
#include <cmath>
int main() { float h[3]{}, s[3]{}; sensor::MovingAverageFilter avg(h, 3); assert(std::fabs(avg.apply(1) - 1) < 0.001F); assert(std::fabs(avg.apply(2) - 1.5F) < 0.001F); assert(std::fabs(avg.apply(6) - 3) < 0.001F); sensor::MedianFilter median(h, s, 3); assert(std::fabs(median.apply(10) - 10) < 0.001F); assert(std::fabs(median.apply(30) - 20) < 0.001F); sensor::KalmanFilter kalman(.01F, 1); assert(std::fabs(kalman.apply(20) - 20) < 0.001F); assert(kalman.apply(24) > 20); }
