#ifndef ODOMETER_H
#define ODOMETER_H

#include <cstdint>

class ImgOdometer
{
public:

    /* ================= 里程数据 ================= */

    double total_pulse = 0.0;   // 总里程

    double last_pulse[7] = {0}; // 启动时记录的脉冲值
    double now_pulse[7]  = {0}; // 当前运行的脉冲增量

    /* ================= 控制接口 ================= */

    /*
     * 函数作用：启动某一路里程计
     * use_odometry: 0~3
     */
    void start(uint8_t use_odometry);

    /*
     * 函数作用：读取某一路里程计当前值
     */
    void update(uint8_t use_odometry);

    /*
     * 函数作用：停止某一路里程计
     */
    void stop(uint8_t use_odometry);

private:

    /* ================= 状态标志 ================= */

    bool odometer_flag[7] = {false};

};

#endif
