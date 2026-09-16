#include "cmsis_os.h"
#include "control_task.hpp"
#include "io/plotter/plotter.hpp"

sp::Plotter plotter(&huart1);

// clang-format off
extern "C" void plot_task()
{
	while (1) {

    plotter.plot(
      rm_motor_x.angle,
      rm_motor_x.speed,
      rm_motor_x.torque
	);

		osDelay(5);
	}
}
// clang-format on