#include "can.hpp"

#include "cmsis_os.h"

void can_start()
{
	can1.config();
	can1.start();
	can2.config();
	can2.start();
}

void send_rm_motor()
{
	rm_motor_x.write(can1.tx_data);
	can1.send(rm_motor_x.tx_id);
}