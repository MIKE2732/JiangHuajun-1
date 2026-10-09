#include "imu_task.hpp"
#include "cmsis_os.h"
#include "data_interfaces/uart/uart.hpp" //遥控
#include "io/bmi088/bmi088.hpp"

// #include "tools/float_to_str.hpp"
#include "io/plotter/plotter.hpp" // 新加
#include "tools/mahony/mahony.hpp"
#include "usart.h"

// 坐标变换矩阵
// 3 列依次是 IMU 坐标系{b}的三个基向量在机器人坐标系{a}下的坐标。
// C 板横装、CAN 一侧朝前时的取值（来自 sp_middleware/io/bmi088/readme.md）。
// BMI088 的输出经过这个矩阵转换后，z 轴才与云台 yaw 轴重合。
const float imu_to_robot[3][3] = {
    {0.0f, -1.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
};

sp::BMI088 bmi088(&hspi1, GPIOA, GPIO_PIN_4, GPIOB, GPIO_PIN_0, imu_to_robot);

// dt 传 1e-3f（1ms），因为任务循环是 osDelay(1)
sp::Mahony mahony(1e-3f);

// 新加，IMU 专用绘图器，用 UART2
sp::Plotter imu_plotter(&huart2, false);

// 打印间隔：50Hz → 20ms → 20 个循环
constexpr uint16_t PRINT_INTERVAL_LOOPS = 20;

// 缓冲区必须是全局/static
// 后续若改用 DMA 异步发送，函数返回后数据还在被读取，
// 局部变量会因栈复用而变成乱码。
// static char print_buf[64];

// 打印三个姿态角
// static void imu_print(float roll_deg, float pitch_deg, float yaw_deg) {
//   uint8_t idx = 0;

//   idx += float_to_str(roll_deg, print_buf + idx, PRINT_DECIMAL);
//   print_buf[idx++] = ',';
//   idx += float_to_str(pitch_deg, print_buf + idx, PRINT_DECIMAL);
//   print_buf[idx++] = ',';
//   idx += float_to_str(yaw_deg, print_buf + idx, PRINT_DECIMAL);
//   print_buf[idx++] = '\r';
//   print_buf[idx++] = '\n';

// 阻塞发送：返回值 HAL_OK 表示已发完
// 超时给 100ms，10Hz 下这个等待绰绰有余
// HAL_UART_Transmit(&huart2, (uint8_t *)print_buf, idx, 100);
// }
//
// 老三通道
static void imu_print(float roll_deg, float pitch_deg, float yaw_deg) {
  imu_plotter.plot(roll_deg, pitch_deg, yaw_deg);
}
// 右拨杆档位转成可绘图的值：上=1.0 / 中=0.0 / 下=-1.0
constexpr float SW_UP = 1.0f;
constexpr float SW_MID = 0.0f;
constexpr float SW_DOWN = -1.0f;
static float remote_value() {
  switch (remote.sw_r) {
  case sp::DBusSwitchMode::UP:
    return SW_UP;
  case sp::DBusSwitchMode::MID:
    return SW_MID;
  default:
    return SW_DOWN;
  }
}
// // 4通道
// static void imu_print(float roll_deg, float pitch_deg, float yaw_deg,
//                       float remote_ch4) {
//   imu_plotter.plot(roll_deg, pitch_deg, yaw_deg, remote_ch4);
// }

// FreeRTOS 任务入口
extern "C" void imu_task() {
  bmi088.init();

  mahony.update(bmi088.acc, bmi088.gyro); // 先喂一次，让内部完成初始化

  uint16_t loop_count = 0;

  while (true) {
    // 读 IMU 原始数据
    bmi088.update();

    // 姿态解算，结果写入 mahony.yaw / pitch / roll（单位：弧度）
    mahony.update(bmi088.acc, bmi088.gyro);

    // 每 20 个循环打印一次（50Hz）
    if (++loop_count >= PRINT_INTERVAL_LOOPS) {
      loop_count = 0;
      imu_print(mahony.roll * RAD_TO_DEG, mahony.pitch * RAD_TO_DEG,
                mahony.yaw * RAD_TO_DEG);
      // imu_print(mahony.roll * RAD_TO_DEG, mahony.pitch * RAD_TO_DEG,
      //           mahony.yaw * RAD_TO_DEG, remote_value());
    }

    osDelay(1); // 1kHz，与 Mahony 的 dt=1e-3 保持一致
  }
}