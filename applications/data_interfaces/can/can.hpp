#ifndef CAN_HPP
#define CAN_HPP

#include "control_task.hpp"
#include "io/can/can.hpp"

inline sp::CAN can1(&hcan1);
inline sp::CAN can2(&hcan2);

void can_start();
void send_rm_motor();

#endif	// CAN_HPP