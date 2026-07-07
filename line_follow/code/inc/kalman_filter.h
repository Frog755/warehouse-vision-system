//
// Created by Taseny on 24-11-18.
//

#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H

#define PI 3.14159265358979

typedef struct Kalman_Filter_structure
{
    double kalman_theta;
    double kalman_omega;
    double P[2][2];
    double Q[2][2];
    double R;
    double K[2];
}Kalman_Filter_structure;

#ifdef __cplusplus
extern "C" {
#endif

void kalman_predict(Kalman_Filter_structure *ptr_structure, float gyro, double dt);
void kalman_update(Kalman_Filter_structure *ptr_structure, double accel_angle);

extern Kalman_Filter_structure axis_y_kalman_filter_structure;
#ifdef __cplusplus
}
#endif
#endif //KALMAN_FILTER_H