// //
// // Created by Taseny on 25-4-6.
// //
//
// #include "../inc/key.h"
// #include "zf_driver_gpio.h"
//
// bool encoderUpdateUPFlag = false;
// bool encoderUpdateDownFlag = false;
//
// uint8_t keyOld = 0;
//
// #define KEY_0       "/dev/zf_driver_gpio_key_0"
// #define KEY_1       "/dev/zf_driver_gpio_key_1"
// #define KEY_2       "/dev/zf_driver_gpio_key_2"
// #define KEY_3       "/dev/zf_driver_gpio_key_3"
//
// uint8_t key_scan(void)
// {
//     uint8_t keyValue = 0;
//     uint8_t keyBuffer = 0xFF;
//
//     keyBuffer = keyBuffer << 1 | !encoderUpdateUPFlag;
//     keyBuffer = keyBuffer << 1 | !encoderUpdateDownFlag;
//     keyBuffer = keyBuffer << 1 | gpio_get_level(KEY_2);
//     keyBuffer = keyBuffer << 1 | gpio_get_level(KEY_3);
//     encoderUpdateUPFlag = false;
//     encoderUpdateDownFlag = false;
//     keyBuffer = ~keyBuffer;
//     switch (keyBuffer)
//     {
//     case KEY_UP: keyValue = KEY_UP; break;
//     case KEY_DOWN: keyValue = KEY_DOWN; break;
//     case KEY_LEFT: keyValue = KEY_LEFT; break;
//     case KEY_RIGHT: keyValue = KEY_RIGHT; break;
//     default: keyValue = INVALID; break;
//     }
//     return keyValue;
// }