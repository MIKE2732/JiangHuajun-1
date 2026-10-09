#include "cmsis_os.h"
#include "io/buzzer/buzzer.hpp"
#include <cstdint>

sp::Buzzer buzzer(&htim4, TIM_CHANNEL_3, 84e6);

uint8_t buzzer_delay = 200;

extern "C" void buzzer_task() {
  bool is_power_on = __HAL_RCC_GET_FLAG(RCC_FLAG_PORRST);
  __HAL_RCC_CLEAR_RESET_FLAGS();

  const float notes[3] = {800.0f, 0.0f, 1200.0f}; // 0 = ??
  int count = is_power_on ? 2 : 3;

  for (int i = 0; i < count; i++) {
    if (notes[i] > 0) {
      buzzer.set(notes[i], 0.05);
      buzzer.start();
    } else {
      buzzer.stop();
    }
    osDelay(buzzer_delay);
  }
  buzzer.stop();

  while (true) {
    osDelay(buzzer_delay);
  }
}