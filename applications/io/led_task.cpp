#include "cmsis_os.h"
#include "io/led/led.hpp"
#include <cstdint>

sp::LED led(&htim5);

// 编译时常量：颜色帧数
constexpr uint32_t LED_INTERVAL_MS = 500;
constexpr float LED_BRIGHTNESS = 0.3f;

// 颜色表：红/绿/蓝 + 各自的间隔
const float led_color[][3] = {
    {LED_BRIGHTNESS, 0, 0}, {0, 0, 0}, {0, LED_BRIGHTNESS, 0}, {0, 0, 0},
    {0, 0, LED_BRIGHTNESS}, {0, 0, 0},
};

extern "C" void led_task() {
  led.start();

  // 启动确认：白色 500ms
  led.set(LED_BRIGHTNESS, LED_BRIGHTNESS, LED_BRIGHTNESS);
  osDelay(500);

  uint8_t color_index = 0;
  while (true) {
    led.set(led_color[color_index][0], led_color[color_index][1],
            led_color[color_index][2]);
    osDelay(LED_INTERVAL_MS);
    color_index =
        (color_index + 1) % (sizeof(led_color) / sizeof(led_color[0]));
  }
}
