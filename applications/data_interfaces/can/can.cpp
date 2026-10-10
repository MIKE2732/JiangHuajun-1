#include "can.hpp"

#include "cmsis_os.h"

void can_start() {
  can1.config();
  can1.start();
  can2.config();
  can2.start();
}

void send_rm_motor() {
  // GM6020 的 ID 1~4 共用帧 0x1FF，各自占用不同的字节位置
  // write() 内部按 (motor_id-1)%4*2 计算偏移，无需手动清零
  rm_motor_x.write(can1.tx_data); // ID 1 → data[0..1]
  rm_motor_y.write(can1.tx_data); // ID 2 → data[2..3]
  can1.send(rm_motor_y.tx_id);    // 一次发送，两台都收到
}
