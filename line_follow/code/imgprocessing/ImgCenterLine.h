#ifndef IMGCENTERLINE_H
#define IMGCENTERLINE_H

#include "ImgConfig.h"
// #include "ImgYolo.h"

class ImgLineTracker;
class ImgElementDetector;

/**
 * @brief 中线决策 + 前瞻计算 + 横向误差计算模块
 */
class ImgCenterLine
{
public:
    ImgCenterLine(ImgLineTracker& tracker,
                  ImgElementDetector& element);

    uint16 points_test_X;
    uint16 points_test_Y;

    /**
     * @brief 执行完整流程
     */
    void process();

    /**
     * @brief 获取当前横向误差
     */
    float error() const;


    float getError() const { return Det_True; }

    /* ===== 内部状态 ===== */
    float Det_True = 0.0f;
    float last_Det_True = 0.0f;

    void calculate_error(uint8 prospect);
    uint8 get_prospect();   // ← 新增前瞻函数

private:

    /* ===== 外部依赖 ===== */
    ImgLineTracker& tracker_;
    ImgElementDetector& element_;


    static constexpr uint8 weight_size = 11; // 参与误差计算的权重


    uint8 Weight2[50] = {
        10, 15, 27, 40, 40, 40 , 40, 40, 27, 15, 10,
        7, 6, 6, 6, 5, 4, 4, 4, 3, 3,
        1, 1,  1,  1,  1,  1,  1,  1,  1,  1,
        8,  8,  6,  4,  2,  2,  2,  2,  1,  1,
        1,  1,  1,  1,  1,
    };


    /* ===== 内部函数 ===== */
    void adjust_center_line();


};

#endif
