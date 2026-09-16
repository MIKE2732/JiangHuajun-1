#include "control_task.hpp"

#include "cmsis_os.h"
#include "data_interfaces/can/can.hpp"
#include "data_interfaces/uart/uart.hpp"
#include "tools/pid/pid.hpp"

GimbalData rm_motor_data;
sp::DBusSwitchMode last_sw_l = remote.sw_l;

//                           dt     kp    ki    kd    mo   mio   alpha  ang? dynamic?
sp::PID rm_motor_pid_angle(0.001f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, true, true);
sp::PID rm_motor_pid_speed(0.001f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, false, true);

extern "C" void control_task()
{
	remote.request();

	osDelay(100);
	can_start();
	osDelay(100);

	// 初始化
	rm_motor_data.target_angle_set = rm_motor_x.angle;
	rm_motor_data.target_speed_set = 0.0f;
	rm_motor_data.given_torque = 0.0f;

	while (true) {
		if (remote.sw_r == sp::DBusSwitchMode::MID) {
			// 单速度环pid
			rm_motor_pid_speed.calc(rm_motor_data.target_speed_set, rm_motor_x.speed);
			rm_motor_data.given_torque = rm_motor_pid_speed.out;

			// 双环pid
			// rm_motor_pid_angle.calc(rm_motor_data.target_angle_set, rm_motor_x.angle);
			// rm_motor_data.target_speed_set = rm_motor_pid_angle.out;
			// rm_motor_pid_speed.calc(rm_motor_data.target_speed_set, rm_motor_x.speed);
			// rm_motor_data.given_torque = rm_motor_pid_speed.out;
		}
		else {
			rm_motor_data.given_torque = 0.0f;
		}

		rm_motor_x.cmd(rm_motor_data.given_torque);
		send_rm_motor();

		last_sw_l = remote.sw_l;

		osDelay(1);
	}
}