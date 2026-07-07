
// 此文件没有用上 后面要用时解开注释后还要移植逐飞库

// #include "zf_common_headfile.hpp"
// #include "../inc/imu.h"
// #include <cmath>   // 加入 math 库头文件
//
// float gyro_x,gyro_y,gyro_z;
// float acc_x ,acc_y ,acc_z ;
// float ax,ay,az;
// float gx,gy,gz;
// float GYRO_Z1=0;
// float Angle_x_temp,Angle_y_temp,Angle_y_temp1;
// float Angle_x_temp_last;
//
// float Pitch1,Roll1;
//
// int16 Acc_x[10];
// int16 Acc_y[10];
// int16 Acc_z[10];
// int16 Gyro_x[10];
// int16 Gyro_y[10];
// int16 Gyro_z[10];
//
// void imu_change(void)
// {
//     Acc.X = averageFilter(imu660ra_acc_x, Acc_x, 10);
//     Acc.Y = averageFilter(imu660ra_acc_y, Acc_y, 10);
//     Acc.Z = averageFilter(imu660ra_acc_z, Acc_z, 10);
//
//     Gyro.X = averageFilter(imu660ra_gyro_x, Gyro_x, 10);
//     Gyro.Y = averageFilter(imu660ra_gyro_y, Gyro_y, 10);
//     Gyro.Z = averageFilter(imu660ra_gyro_z, Gyro_z, 10);
//
//     acc_x = (9.8f * Acc.X) / 8192;
//     acc_y = (9.8f * Acc.Y) / 8192;
//     acc_z = (9.8f * Acc.Z) / 8192;
//
//     gyro_x = Gyro.X / 16.4f;
//     gyro_y = Gyro.Y / 16.4f;
//     gyro_z = Gyro.Z / 16.4f;
//
//     Prepare_Data(&Acc, &Acc_out);
//     IMUupdate(&Gyro, &Acc_out, &Att_Angle);
// }
//
// #define Kp 1.73f
// #define Ki 0.001f
// #define halfT 0.001f
//
// #define Gyro_Gr 0.0010653f
// #define Gyro_G  0.9400351f
//
// #define FILTER_NUM  20
//
// float AngleOffset_Rol = 0, AngleOffset_Pit = 0;
// float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
// float exInt = 0, eyInt = 0, ezInt = 0;
//
// FLOAT_XYZ Acc, Acc_out, Gyro;
// FLOAT_ANGLE Att_Angle;
//
// void Prepare_Data(FLOAT_XYZ *acc_in, FLOAT_XYZ *acc_out)
// {
//     static uint8_t filter_cnt = 0;
//     static int16_t ACC_X_BUF[FILTER_NUM], ACC_Y_BUF[FILTER_NUM], ACC_Z_BUF[FILTER_NUM];
//     int32_t temp1 = 0, temp2 = 0, temp3 = 0;
//     uint8_t i;
//
//     ACC_X_BUF[filter_cnt] = acc_in->X;
//     ACC_Y_BUF[filter_cnt] = acc_in->Y;
//     ACC_Z_BUF[filter_cnt] = acc_in->Z;
//
//     for (i = 0; i < FILTER_NUM; i++)
//     {
//         temp1 += ACC_X_BUF[i];
//         temp2 += ACC_Y_BUF[i];
//         temp3 += ACC_Z_BUF[i];
//     }
//     acc_out->X = temp1 / FILTER_NUM;
//     acc_out->Y = temp2 / FILTER_NUM;
//     acc_out->Z = temp3 / FILTER_NUM;
//
//     filter_cnt++;
//     if (filter_cnt == FILTER_NUM) filter_cnt = 0;
// }
//
// void IMUupdate(FLOAT_XYZ *gyr, FLOAT_XYZ *acc, FLOAT_ANGLE *angle)
// {
//     float ax = acc->X, ay = acc->Y, az = acc->Z;
//     float gx = gyr->X, gy = gyr->Y, gz = gyr->Z;
//     float norm;
//
//     float vx, vy, vz;
//     float ex, ey, ez;
//
//     float q0q0 = q0*q0;
//     float q0q1 = q0*q1;
//     float q0q2 = q0*q2;
//     float q1q1 = q1*q1;
//     float q1q3 = q1*q3;
//     float q2q2 = q2*q2;
//     float q2q3 = q2*q3;
//     float q3q3 = q3*q3;
//
//     if (ax*ay*az == 0)
//         return;
//
//     gx *= Gyro_Gr;
//     gy *= Gyro_Gr;
//     gz *= Gyro_Gr;
//
//     norm = std::sqrt(ax*ax + ay*ay + az*az);
//     ax /= norm;
//     ay /= norm;
//     az /= norm;
//
//     vx = 2 * (q1q3 - q0q2);
//     vy = 2 * (q0q1 + q2q3);
//     vz = q0q0 - q1q1 - q2q2 + q3q3;
//
//     ex = (ay * vz - az * vy);
//     ey = (az * vx - ax * vz);
//     ez = (ax * vy - ay * vx);
//
//     exInt += ex * Ki;
//     eyInt += ey * Ki;
//     ezInt += ez * Ki;
//
//     gx += Kp * ex + exInt;
//     gy += Kp * ey + eyInt;
//     gz += Kp * ez + ezInt;
//
//     q0 += (-q1 * gx - q2 * gy - q3 * gz) * halfT;
//     q1 += (q0 * gx + q2 * gz - q3 * gy) * halfT;
//     q2 += (q0 * gy - q1 * gz + q3 * gx) * halfT;
//     q3 += (q0 * gz + q1 * gy - q2 * gx) * halfT;
//
//     norm = std::sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
//
//     q0 /= norm;
//     q1 /= norm;
//     q2 /= norm;
//     q3 /= norm;
//
//     angle->yaw += -gyr->Z * Gyro_G * 0.002f;
//     angle->rol = -std::asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3f - AngleOffset_Pit;
//     angle->pit = -std::atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3f - AngleOffset_Rol;
// }
//
// int16 averageFilter(int16 in_data, int16 data[], int16 count)
// {
//     float sum = 0;
//     for (int i = 0; i < count - 1; i++)
//     {
//         data[i] = data[i + 1];
//         sum += data[i];
//     }
//     data[count - 1] = in_data;
//     sum += in_data;
//
//     return sum / count;
// }
//
