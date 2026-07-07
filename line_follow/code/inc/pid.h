// #ifndef PID_H
// #define PID_H
//
// #define bang_error_max  200.0f
// #define ki_error_max    150.0f
// #define ki_value_max    7000.0f
// #define k_out_max       7000.0f
//
// // 增量式 PID 控制器类（用于左右轮电机控制）
// class IncrementalPID {
// public:
//     IncrementalPID(float Kp, float Ki, float Kd);
//     float compute(float setpoint, float measured_value);
//
//     void setParameters(float Kp, float Ki, float Kd);
//     void setKp(float Kp);
//     void setKi(float Ki);
//     void setKd(float Kd);
//
//     float Kp, Ki, Kd;
//     float k_out, kp_value, ki_value, kd_value;
//
// private:
//     float prev_error, prev_prev_error, error;
//
//     // ADRC TD相关变量
//     float v1, v2;
//     float h, r, N0;
//
//     void trackingDifferentiator(float target);
//     float fhan(float x1, float x2, float r, float h);
//     float sign(float x);
// };
//
// // 位置式 PID 控制器类（用于舵机控制）
// class PositionPID {
// public:
//     PositionPID(float Kp, float Kp2, float Kd, float Kd2);
//     float compute(float position, float gyroz);
//
//     float Kp_Out;
//     float Kd_Out;
//
//     void setParameters(float Kp, float Kp2, float Kd, float Kd2);
//     void setKp(float Kp);
//     void setKp2(float Kp2);
//     void setKd(float Kd);
//     void setKd2(float Kd2);
//
// private:
//     float Kp, Kp2, Kd, Kd2;
//     float prev_position;
// };
//
// #endif // PID_H
