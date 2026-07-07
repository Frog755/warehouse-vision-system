#ifndef PID__H
#define PID__H

// 增量式 PID 控制器类（用于左右轮电机控制）
class IncrementalPID {
public:
    IncrementalPID(float Kp, float Ki, float Kd);
    int compute(int setpoint, int measured_value);

    float Kp, Ki, Kd;

private:
    int prev_error, prev_prev_error, error;
};

// 方向环 位置式 PID 控制器类
class PositionPID {
public:
    PositionPID(float Kp, float Kd);
    float compute(float position);

    float Kp, Kd;

private:
    float prev_position;
};

// 角速度环 位置式 PID 控制器类
class GyrozPID {
public:
    GyrozPID(float Kp, float Kd);
    float compute(float position, float target);

    float Kp, Kd;

private:
    float prev_position;
};

#endif // PID_H

