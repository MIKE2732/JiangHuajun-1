#include "tools/float_to_str.hpp"

uint8_t float_to_str(float value, char *buf, uint8_t decimal) {
  uint8_t idx = 0; // buf 当前写入位置

  // 判断符号并取绝对值
  bool is_negative = value < 0;
  if (is_negative) {
    value = -value;
  }

  // 循环累乘算10^decimal
  int32_t scale = 1;
  for (uint8_t i = 0; i < decimal; i++) {
    scale *= 10;
  }

  // 缩放 + 四舍五入，必须写成一个表达式
  int32_t fixed = (int32_t)(value * scale + 0.5f);

  // 拆成整数部分和小数部分
  int32_t digit = fixed / scale; // 整数部分
  int32_t frac = fixed % scale;  // 小数部分的数值

  // 整数部分转字符串（倒序提取，再倒过来写）
  char temp[16];
  uint8_t len = 0;

  if (digit == 0) {
    // 特判 0：while 循环不会执行，否则整数部分会是空字符串
    temp[len++] = '0';
  } else {
    while (digit > 0) {
      temp[len++] = (char)('0' + digit % 10);
      digit /= 10;
    }
  }

  // 倒序写入 buf
  for (uint8_t i = 0; i < len; i++) {
    buf[idx++] = temp[len - 1 - i];
  }

  // 小数部分：仅当 decimal > 0 时才需要
  if (decimal > 0) {
    buf[idx++] = '.';

    if (frac == 0) {
      // 特判 0，直接补满 decimal 个 0
      for (uint8_t i = 0; i < decimal; i++) {
        buf[idx++] = '0';
      }
    } else {
      len = 0;
      while (frac > 0) {
        temp[len++] = (char)('0' + frac % 10);
        frac /= 10;
      }

      // 补前导零：0.005 应输出 "005" 而不是 "5"
      for (uint8_t i = len; i < decimal; i++) {
        temp[len++] = '0';
      }

      for (uint8_t i = 0; i < len; i++) {
        buf[idx++] = temp[len - 1 - i];
      }
    }
  }

  // 负号放在最前面
  // 不能一开始就写，因为那时还不知道符号
  if (is_negative) {
    // 把已有内容整体右移一位，给负号腾位置
    for (uint8_t i = idx; i > 0; i--) {
      buf[i] = buf[i - 1];
    }
    buf[0] = '-';
    idx++;
  }

  buf[idx] = '\0'; // 结束符
  return idx;
}