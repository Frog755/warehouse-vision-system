//
// Created by Taseny on 25-4-6.
//
#pragma once
#ifndef KEY_HPP          // ✅ 改为 KEY_HPP，与文件名一致
#define KEY_HPP

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum KEY_LIST
{
    INVALID,
    KEY_UP = 0x08,
    KEY_DOWN = 0x04,
    KEY_LEFT = 0x02,
    KEY_RIGHT = 0x01,
};

uint8_t key_scan(void);

#ifdef __cplusplus
}
#endif
extern bool encoderUpdateUPFlag;
extern bool encoderUpdateDownFlag;
extern uint8_t keyOld;
#endif // KEY_HPP