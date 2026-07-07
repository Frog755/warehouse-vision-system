//
// Created by qing on 25-5-27.
//
#include "../inc/img_math.h"
template<typename T>
T clamp(T v, T lo, T hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}
//计算三个点的夹角
double computeFullAngle2D(uint8_t x1, uint8_t y1,
                          uint8_t x2, uint8_t y2,
                          uint8_t x3, uint8_t y3) {
    // 转换为 double 类型，防止整数除法和溢出
    double dx1 = static_cast<double>(x2) - static_cast<double>(x1);
    double dy1 = static_cast<double>(y2) - static_cast<double>(y1);
    double dx2 = static_cast<double>(x3) - static_cast<double>(x1);
    double dy2 = static_cast<double>(y3) - static_cast<double>(y1);

    double dot = dx1 * dx2 + dy1 * dy2;
    double len1 = std::sqrt(dx1 * dx1 + dy1 * dy1);
    double len2 = std::sqrt(dx2 * dx2 + dy2 * dy2);

    if (len1 == 0.0 || len2 == 0.0) {
        std::cerr << "错误：向量长度为 0，无法计算夹角！" << std::endl;
        return 0.0;
    }

    double cos_angle = dot / (len1 * len2);
    cos_angle = clamp(cos_angle, -1.0, 1.0); // 避免浮点精度误差
    double angle_rad = std::acos(cos_angle);
    return angle_rad * 180.0 / M_PI;
}