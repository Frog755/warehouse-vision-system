
// 此文件没有用上 后面要用时解开注释后还要移植逐飞库

// #ifndef _IMU_H_
// #define _IMU_H_
//
// #define G           9.80665f                // 重力加速度 m/s^2
// #define RadtoDeg    57.324841f              // 弧度转角度 (rad × 180/π)
// #define DegtoRad    0.0174533f              // 角度转弧度 (deg × π/180)
//
// #include "zf_common_headfile.hpp"
//
// // 三轴浮点/整数值结构体
// typedef struct
// {
//     int16 X;
//     int16 Y;
//     int16 Z;
// } FLOAT_XYZ;
//
// // 姿态欧拉角结构体
// typedef struct
// {
//     float rol;
//     float pit;
//     float yaw;
// } FLOAT_ANGLE;
//
// // 外部全局变量声明
// extern FLOAT_XYZ Acc, Acc_out, Gyro;
// extern FLOAT_ANGLE Att_Angle;
//
// extern float gyro_x, gyro_y, gyro_z;
// extern float acc_x, acc_y, acc_z;
//
// extern float Pitch1, Roll1;
//
// // 函数声明
// void Prepare_Data(FLOAT_XYZ *acc_in, FLOAT_XYZ *acc_out);
// void IMUupdate(FLOAT_XYZ *Gyr_rad, FLOAT_XYZ *Acc_filt, FLOAT_ANGLE *Att_Angle);
// void imu_change(void);
// void imu_display(void);
// int16 averageFilter(int16 in_data, int16 data[], int16 count);
//
// #endif
