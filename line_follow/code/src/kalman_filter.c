//
// Created by Taseny on 24-11-18.
//

#include "../inc/kalman_filter.h"

/*
* */
Kalman_Filter_structure axis_y_kalman_filter_structure = {
    .kalman_theta = 0,
    .kalman_omega = 0,
    /*状态协方差矩阵*/
    .P[0][0] = 1,
    .P[0][1] = 0,
    .P[1][0] = 0,
    .P[1][1] = 1,
    /*过程噪声*/
    .Q[0][0] = 0.001,
    .Q[0][1] = 0,
    .Q[1][0] = 0,
    .Q[1][1] = 0.003,

    .R = 0.03,
    .K[0] = 0,
    .K[1] = 0
};

// 预测阶段
void kalman_predict(Kalman_Filter_structure *ptr_structure, const float gyro, const double dt) {
    // 状态预测
    ptr_structure->kalman_theta += ptr_structure->kalman_omega * dt;
    ptr_structure->kalman_omega = gyro;

    // 协方差预测
    ptr_structure->P[0][0] += dt * (ptr_structure->P[1][0] + ptr_structure->P[0][1] + dt * ptr_structure->P[1][1]) + ptr_structure->Q[0][0];
    ptr_structure->P[0][1] += dt * ptr_structure->P[1][1];
    ptr_structure->P[1][0] += dt * ptr_structure->P[1][1];
    ptr_structure->P[1][1] += ptr_structure->Q[1][1];
}

// 更新阶段
void kalman_update(Kalman_Filter_structure *ptr_structure, const double accel_angle) {
    // 卡尔曼增益计算
    const double S = ptr_structure->P[0][0] + ptr_structure->R;
    ptr_structure->K[0] = ptr_structure->P[0][0] / S;
    ptr_structure->K[1] = ptr_structure->P[1][0] / S;

    // 状态更新
    ptr_structure->kalman_theta += ptr_structure->K[0] * (accel_angle - ptr_structure->kalman_theta);
    ptr_structure->kalman_omega += ptr_structure->K[1] * (accel_angle - ptr_structure->kalman_theta);

    // 协方差更新
    ptr_structure->P[0][0] -= ptr_structure->K[0] * ptr_structure->P[0][0];
    ptr_structure->P[0][1] -= ptr_structure->K[0] * ptr_structure->P[0][1];
    ptr_structure->P[1][0] -= ptr_structure->K[1] * ptr_structure->P[0][0];
    ptr_structure->P[1][1] -= ptr_structure->K[1] * ptr_structure->P[0][1];
}