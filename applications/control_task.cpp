#include "control_task.hpp"

#include "cmsis_os.h"
#include "data_interfaces/can/can.hpp"
#include "data_interfaces/uart/uart.hpp"
#include "tools/math_tools/math_tools.hpp"
#include "tools/pid/pid.hpp"

GimbalData motor_data;

// 两台电机各一个速度环
sp::PID pid_speed_a(0.001f, PID_KP, PID_KI, PID_KD, PID_MAXOUT, PID_MAXIOUT,
                    1.0f, false, true);
sp::PID pid_speed_b(0.001f, PID_KP, PID_KI, PID_KD, PID_MAXOUT, PID_MAXIOUT,
                    1.0f, false, true);

// 读取左拨杆档位对应的联动比例
static float get_ratio() {
  switch (remote.sw_l) {
  case sp::DBusSwitchMode::UP:
    return RATIO_HIGH; // 1:3
  case sp::DBusSwitchMode::MID:
    return RATIO_MID; // 1:1
  default:
    return RATIO_LOW; // 1:0.5
  }
}

// 力矩限幅
static float limit_torque(float torque) {
  if (torque > TORQUE_LIMIT)
    return TORQUE_LIMIT;
  if (torque < -TORQUE_LIMIT)
    return -TORQUE_LIMIT;
  return torque;
}

// IMU yaw 累计角（解环绕，避免 ±180跳变）
// static sp::AngleUnwrapper yaw_unwrapper;
static float yaw_unwrapped = 0.0f;
static float yaw_reference = 0.0f; // 联动参考零点

extern "C" void control_task() {
  remote.request();

  osDelay(100);
  can_start();
  osDelay(100);

  motor_data.target_speed_set = 0.0f;
  motor_data.torque_a = 0.0f;
  motor_data.torque_b = 0.0f;

  while (true) {
    // 目标速度为 0 时清空 PID 状态，避免陈旧积分导致电机自转
    if (motor_data.torque_b == 0.0f) {
      pid_speed_b.clear();
    }
    // ---- 右拨杆三档模式 ----
    switch (remote.sw_r) {
    case sp::DBusSwitchMode::DOWN:
      // 失能模式：两台电机无力
      motor_data.torque_a = 0.0f;
      motor_data.torque_b = 0.0f;
      break;

    case sp::DBusSwitchMode::MID: {
      // 联动模式：速度比跟随
      float ratio = get_ratio();

      // A 电机：开环给0 力矩，由人手动转动
      motor_data.torque_a = 0.0f;

      // B 电机：速度环跟随 A 电机的实际速度 ×比例
      float target_speed_b = rm_motor_x.speed * ratio;
      pid_speed_b.calc(target_speed_b, rm_motor_y.speed);
      motor_data.torque_b = limit_torque(pid_speed_b.out);
      break;
    }

    case sp::DBusSwitchMode::UP:
      // 复位模式：两台电机无力，零点归零
      motor_data.torque_a = 0.0f;
      motor_data.torque_b = 0.0f;
      yaw_reference = yaw_unwrapped;
      break;
    }

    // ---- 下发电机指令 ----
    rm_motor_x.cmd(motor_data.torque_a);
    rm_motor_y.cmd(motor_data.torque_b);
    send_rm_motor();

    osDelay(1);
  }
}
