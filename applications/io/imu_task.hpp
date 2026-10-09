#ifndef IMU_TASK_HPP
#define IMU_TASK_HPP

#include <cstdint>

// 弧度转度的系数：180 / π
// -O2 编译时会优化成一次乘法，比每次做除法快
constexpr float RAD_TO_DEG = 57.2957795f;

// 打印内容的小数位数
constexpr uint8_t PRINT_DECIMAL = 3;

#endif // IMU_TASK_HPP