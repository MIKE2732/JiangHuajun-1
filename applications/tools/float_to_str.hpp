#ifndef FLOAT_TO_STR_HPP
#define FLOAT_TO_STR_HPP

#include <cstdint>

// 把 value 转成定点数字符串写入 buf，返回写入的字符数(不含结束符)
// 支持负数，固定 decimal 位小数
uint8_t float_to_str(float value, char *buf, uint8_t decimal = 3);

#endif // FLOAT_TO_STR_HPP