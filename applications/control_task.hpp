#ifndef CONTROL_TASK_HPP
#define CONTROL_TASK_HPP

#include "motor/rm_motor/rm_motor.hpp"

// A 电机（基准）ID 1，B 电机（跟随）ID 2
inline sp::RM_Motor rm_motor_x(1, sp::RM_Motors::GM6020);
inline sp::RM_Motor rm_motor_y(2, sp::RM_Motors::GM6020);

// 左拨杆三档联动比例：1:0.5 / 1:1 / 1:3
constexpr float RATIO_LOW = 0.5f;
constexpr float RATIO_MID = 1.0f;
constexpr float RATIO_HIGH = 3.0f;

// 内环速度环PID 参数（明天实测调整）
constexpr float PID_KP = 5.0f;
constexpr float PID_KI = 0.5f;
constexpr float PID_KD = 0.0f;
constexpr float PID_MAXOUT = 100.0f; // 力矩上限，安全保护
constexpr float PID_MAXIOUT = -100.0f;

// 力矩上限（GM6020 最大2.22 N·m，调试期必须限制）
constexpr float TORQUE_LIMIT = 100.0f;

typedef struct {
  float target_speed_set; // A 电机目标速度 rad/s
  float torque_a;         // A 电机力矩
  float torque_b;         // B 电机力矩
} GimbalData;

#endif // CONTROL_TASK_HPP
