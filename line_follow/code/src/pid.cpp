// #include "../inc/pid.h"
// #include <cmath>
//
// // 构造函数
// IncrementalPID::IncrementalPID(float Kp_, float Ki_, float Kd_)
//     : Kp(Kp_), Ki(Ki_), Kd(Kd_), prev_error(0), prev_prev_error(0),
//       error(0), kp_value(0), ki_value(0), kd_value(0), k_out(0),
//       v1(0), v2(0), h(0.005f), r(40.0f), N0(5.0f) {}
//
// // 设置 PID 参数
// void IncrementalPID::setParameters(float Kp_, float Ki_, float Kd_) {
//     Kp = Kp_;
//     Ki = Ki_;
//     Kd = Kd_;
// }
//
// void IncrementalPID::setKp(float Kp_) { Kp = Kp_; }
// void IncrementalPID::setKi(float Ki_) { Ki = Ki_; }
// void IncrementalPID::setKd(float Kd_) { Kd = Kd_; }
//
// // 主控制函数
// float IncrementalPID::compute(float setpoint, float measured_value) {
//     trackingDifferentiator(setpoint); // TD用于微分项
//
//     error = setpoint - measured_value;
//
//     if (error > bang_error_max)
//     {
//         k_out = k_out_max;
//     }
//     else if (error < -bang_error_max)
//     {
//         k_out = -k_out_max;
//     }
//     else
//     {
//         kp_value = Kp * error;
//
//         if (error < ki_error_max && error > -ki_error_max)
//         {
//             ki_value += Ki * error;
//             if (ki_value > ki_value_max) ki_value = ki_value_max;
//             if (ki_value < -ki_value_max) ki_value = -ki_value_max;
//         }
//         else
//         {
//             ki_value = 0;
//         }
//
//         kd_value = Kd * v2; // TD导数作为微分项
//         k_out = kp_value + ki_value + kd_value;
//     }
//
//     if (k_out > k_out_max) k_out = k_out_max;
//     if (k_out < -k_out_max) k_out = -k_out_max;
//
//     return k_out;
// }
//
// // ADRC Tracking Differentiator
// void IncrementalPID::trackingDifferentiator(float target) {
//     float fh = v2;
//     v1 += h * fh;
//     v2 += h * fhan(target - v1, v2, r, h);
// }
//
// // 非线性函数 fhan
// float IncrementalPID::fhan(float x1, float x2, float r, float h) {
//     float d = r * h * h;
//     float a0 = h * x2;
//     float y = x1 + a0;
//     float a1 = sqrt(d * (d + 8.0f * fabs(y)));
//     float a2 = a0 + sign(y) * (a1 - d) / 2.0f;
//     return -a2 / h;
// }
//
// // 符号函数
// float IncrementalPID::sign(float x) {
//     return (x > 1e-6f) - (x < -1e-6f);
// }
//
// // ----------------------- PositionPID -----------------------
//
// PositionPID::PositionPID(float Kp, float Kp2, float Kd, float Kd2)
//     : Kp(Kp), Kp2(Kp2), Kd(Kd), Kd2(Kd2), prev_position(0.0f) {}
//
// float PositionPID::compute(float position, float gyroz) {
//     float output =
//         position * Kp +
//         std::abs(position) * position * Kp2 +
//         (position - prev_position) * Kd +
//         gyroz * Kd2
//     ;
//
//     Kd_Out = position - prev_position;
//     prev_position = position;
//
//     return output;
// }
//
// void PositionPID::setParameters(float Kp, float Kp2, float Kd, float Kd2) {
//     this->Kp = Kp;
//     this->Kp2 = Kp2;
//     this->Kd = Kd;
//     this->Kd2 = Kd2;
// }
// void PositionPID::setKp(float Kp) { this->Kp = Kp; }
// void PositionPID::setKp2(float Kp2) { this->Kp2 = Kp2; }
// void PositionPID::setKd(float Kd) { this->Kd = Kd; }
// void PositionPID::setKd2(float Kd2) { this->Kd2 = Kd2; }
//
//
//
