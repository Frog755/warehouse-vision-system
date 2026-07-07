#ifndef LADRC_H
#define LADRC_H

// LADRC 控制器类（速度环替代 PID）
class LADRC {
public:
    // 构造函数：初始化所有参数
    LADRC(float wc, float w0, float b, float Ts, float limit);

    // LADRC 主计算函数
    float compute(float target, float measured_value);

    // 公有参数（可在主程序中动态调整）
    float wc;      // 控制器带宽
    float w0;      // ESO 带宽
    float b;       // 控制增益
    float Ts;      // 采样周期
    float limit;   // 限幅

private:
    // ESO 状态
    float z1;      // 状态估计
    float z2;      // 扰动估计

    float last_u;  // 上次输出
};

#endif // LADRC_H