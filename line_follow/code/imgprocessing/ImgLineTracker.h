//
// Created by ros on 2026/1/28.
//

#ifndef IMG_LINE_TRACKER_H
#define IMG_LINE_TRACKER_H

#include "ImgConfig.h"
#include <opencv2/opencv.hpp>
#include "zf_common_headfile.hpp"

/* ================= 配置宏 ================= */

constexpr int START_H = RESULT_ROWS - 2; // 98 八领域起始点高一行，避免边缘越界

// #define LINE_TRACKER_DEBUG 1            // 设为 0 可关闭调试接口
#define LINE_TRACKER_DEBUG 0            // 设为 0 可关闭调试接口

constexpr int BORDER_MIN = 1; //边界宽度
constexpr int BORDER_MAX = USED_COL - 1;//边界宽度

/**
 * @brief 赛道寻线模块（纯算法）
 *
 * 职责：
 *  1. 八邻域搜索左右边线
 *  2. 最长白列
 */
class ImgLineTracker
{
public:
    ImgLineTracker();

    /**
     * @brief 执行寻线
     * @param bin_image 输入二值化图像
     */
    // 放在pubilc中，其他.cpp要用

    /*-------八领域 search_l_r-------*/
    uint16 data_stastics_l = 0;    //统计找到点的个数
    uint16 data_stastics_r = 0;
    uint8_t l_border_[USE_H] = {};         // 第 i 行图像的 左边界 x 坐标
    uint8_t r_border_[USE_H] = {};         // 第 i 行图像的 右边界 x 坐标

    uint8_t border_l_[USE_H] = {};         // 用于坡道 放置左边线信息
    uint8_t border_r_[USE_H] = {};         // 用于坡道 放置右边线信息
    uint8 hightest_ = 0;                   // 八领域图像最高点

    uint8_t center_line_[USE_H] = {};      // 中线
    uint8_t center_line_last_[USE_H] = {}; // 上一帧中线

    /*-------最长白列-------*/

    uint8_t l_border_lostnum = 0; // 左边丢线数
    uint8_t r_border_lostnum = 0; // 右边丢线数
    uint8_t lost_count  = 0;      // 双边同时丢线
    uint8_t valid_count = 0;      // 双边同时有线

    uint8 line_detect_left  = 0;//最左长列起始行
    uint8 line_detect_right = 0;//最右长列起始行
    uint8_t line_detect_center = 0; // 最长列起始行

    uint8 r_border_begin = 255; // 右边界的起始点（图像从上往下）
    uint8 l_border_begin = 255; // 左边界的起始点（图像从上往下）
    uint8 border_begin   = 255; // 边线起始点    (从上往下)

    bool l_border_continuous = false;   ///< 左边界从起始行向下是否连续（无断裂/跳变）
    bool r_border_continuous = false;   ///< 右边界从起始行向下是否连续

    /*-------边线起点变化记录-------*/
    uint8_t last_r_border_begin = 255;  // 上一帧右边界起点
    uint8_t last_l_border_begin = 255;  // 上一帧左边界起点
    uint8_t last_border_begin   = 255;  // 上一帧综合边线起点

    int16_t delta_r_border_begin = 0;   // 右边界起点变化值（当前 - 上一帧）
    int16_t delta_l_border_begin = 0;   // 左边界起点变化值
    int16_t delta_border_begin   = 0;   // 综合边线起点变化值

    bool use_right_border_only_ = false;//只以右边来巡线
    bool use_left_border_only_  = false;//只以左边来巡线


    void process(const cv::Mat& bin_image);

    /* ========== 正式输出接口 ========== */

    const uint8_t* getLeftBorder() const { return l_border_; }
    const uint8_t* getRightBorder() const { return r_border_; }
    const uint8_t* getCenterLine() const { return center_line_; }


    void get_left (uint16_t total_L);
    void get_right(uint16_t total_R);

    void left_get (uint16_t total_L);
    void right_get(uint16_t total_R);


    bool fit_and_correct_border(uint8_t* border); // 最小二乘拟合边线

private:
    /* ========== 内部数据 ========== */

    /*寻找八领域起始点 get_start_point*/
    uint8 start_point_l_[2] = { 0 };//八领域种子生长开始的点
    uint8 start_point_r_[2] = { 0 };

    /*-------八领域 search_l_r-------*/
    uint16 points_l[(uint16)USE_NUM][2] = {{0}};    //中心点
    uint16 points_r[(uint16)USE_NUM][2] = {{0}};
    uint16 dir_l[(uint16)USE_NUM] = { 0 };         // 左方向
    uint16 dir_r[(uint16)USE_NUM] = { 0 };         // 右方向

    /* -------最长白列-------*/


    /* ========== 核心算法函数 ========== */
    void CountLostLines();  // 统计丢失的边线

    uint8_t get_start_point(const cv::Mat& ipm_image); // 获取八领域起始点

    void search_l_r(uint16_t break_flag,
                    const cv::Mat& image,
                    uint8_t l_start_x,
                    uint8_t l_start_y,
                    uint8_t r_start_x,
                    uint8_t r_start_y,
                    uint8_t* hightest);  // 八领域搜索左右边线



    void vgetborder_maxwhitecols(const cv::Mat& ipm_image); // 获取最长白列

};

#endif // IMG_LINE_TRACKER_H
