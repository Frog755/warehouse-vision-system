//
// Created by Taseny on 25-4-6.
//

#include "zf_driver_file_buffer.hpp"   // 基类（必须！）
#include "zf_driver_gpio.hpp"   // 然后包含完整定义
#include "../inc/key.hpp"
#include "zf_common_headfile.hpp"

// ========== 1. 先定义所有宏（必须在对象声明之前）==========

// 按键设备路径（直接使用字符串，或从 zf_driver_gpio.hpp 的宏）
#define KEY_1_PATH        ZF_GPIO_KEY_4
#define KEY_2_PATH        ZF_GPIO_KEY_3



// ========== 2. 全局变量 ==========
bool encoderUpdateUPFlag = false;
bool encoderUpdateDownFlag = false;
uint8_t keyOld = 0;

// ========== 3. 声明 GPIO 对象（宏定义之后）==========

zf_driver_gpio  key_1(KEY_1_PATH, O_RDWR);
zf_driver_gpio  key_2(KEY_2_PATH, O_RDWR);


// ========== 4. key_scan 函数 ==========
uint8_t key_scan(void)
{
    uint8_t keyValue = 0;
    uint8_t keyBuffer = 0xFF;

    // ✅ 使用对象方法读取 GPIO
    keyBuffer = keyBuffer << 1 | !encoderUpdateUPFlag;
    keyBuffer = keyBuffer << 1 | !encoderUpdateDownFlag;
    keyBuffer = keyBuffer << 1 |  key_1.get_level();   // ✅ 改这里
    keyBuffer = keyBuffer << 1 |  key_2.get_level();   // ✅ 改这里

    encoderUpdateUPFlag = false;
    encoderUpdateDownFlag = false;

    keyBuffer = ~keyBuffer;
    switch (keyBuffer)
    {
    case KEY_UP:    keyValue = KEY_UP;    break;
    case KEY_DOWN:  keyValue = KEY_DOWN;  break;
    case KEY_LEFT:  keyValue = KEY_LEFT;  break;
    case KEY_RIGHT: keyValue = KEY_RIGHT; break;
    default:        keyValue = INVALID;    break;
    }
    return keyValue;
}