//
// Created by ros on 2026/2/15.
//

#include "ImgOdometer.h"
#include <iostream>

/* ================= 启动里程计 ================= */
void ImgOdometer::start(uint8_t use_odometry)
{
    if (use_odometry >= 7)
    {
        std::cerr << "Odometer::start -> invalid index" << std::endl;
        return;
    }

    // 原逻辑：如果未启动才记录
    if (odometer_flag[use_odometry] == false)
    {
        last_pulse[use_odometry] = total_pulse;
        odometer_flag[use_odometry] = true;
    }
}


/* ================= 更新里程计 ================= */
void ImgOdometer::update(uint8_t use_odometry)
{
    if (use_odometry >= 7)
    {
        std::cerr << "Odometer::update -> invalid index" << std::endl;
        return;
    }

    // 原逻辑没有判断 flag，这里保持严谨性
    if (!odometer_flag[use_odometry])
    {
        return;
    }

    now_pulse[use_odometry] =
        total_pulse - last_pulse[use_odometry];
}


/* ================= 停止里程计 ================= */
void ImgOdometer::stop(uint8_t use_odometry)
{
    if (use_odometry >= 7)
    {
        std::cerr << "Odometer::stop -> invalid index" << std::endl;
        return;
    }

    last_pulse[use_odometry] = 0.0;
    now_pulse[use_odometry]  = 0.0;
    odometer_flag[use_odometry] = false;
}
