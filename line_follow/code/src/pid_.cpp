#include "../inc/pid_.h"

#include <tgmath.h>

// ----------------------- 速度环 IncrementalPID -----------------------
// corr_l = pid_left_ctrl->compute(v_target_left, v_left);

IncrementalPID::IncrementalPID(float Kp, float Ki, float Kd)
    : Kp(Kp), Ki(Ki), Kd(Kd), prev_error(0.0), prev_prev_error(0.0) {}

int IncrementalPID::compute(int setpoint, int measured_value) {
    error = setpoint - measured_value;

    int increment = static_cast<int>(Kp * (error - prev_error) +
        Ki * error +
        Kd * (error - 2 * prev_error + prev_prev_error));

    prev_prev_error = prev_error;
    prev_error = error;

    return increment;
}

// ----------------------- 方向环PositionPID -----------------------
// corr_position = pid_position->compute(static_cast<float>(error0)); //方向环计算
// 期望速度就是0
PositionPID::PositionPID(float Kp, float Kd)
    : Kp(Kp), Kd(Kd), prev_position(0.0f) {}

float PositionPID::compute(float position) {
    float output =
        position * Kp +
        (position - prev_position) * Kd
    ;

    prev_position = position;

    return output;
}

// ----------------------- GYROZPID -----------------------
//    corr_gyroz = pid_gyroz->compute(-imu_gyro_z, -corr_position);
//    //角速度环计算 方向环输出作为角速度环的输入 方向环串角速度环 输入两个负数改变极性

GyrozPID::GyrozPID(float Kp, float Kd)
    : Kp(Kp), Kd(Kd), prev_position(0.0f) {}

float GyrozPID::compute(float position, float target) {
    float error = position - target;
    float output =
        error * Kp +
        (error - prev_position) * Kd
    // +
    // error * 0.0001 * fabs(error)
    ;

    prev_position = error;

    return output;
}




