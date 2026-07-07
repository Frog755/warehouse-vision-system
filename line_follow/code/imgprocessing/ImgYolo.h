#ifndef IMG_YOLO_H
#define IMG_YOLO_H

#include <opencv2/opencv.hpp>
#include <vector>

#include "ImgLineTracker.h"
#include "ImgElementDetector.h"
#include "ImgOdometer.h"
#include "ImgCenterLine.h"

class ImgYolo
{
public:

    ImgYolo(ImgLineTracker& tracker,
            ImgElementDetector& element,
            ImgOdometer& odometer,
            ImgCenterLine& centerline);

    /* ========= 红色检测相关 ========= */
    const int turn_order[6][3] = {{+10,0,-10},{20,-20,0},{0,+20,-20},{0,-20,+20},{-18,0,+18},{-20,+20,0}
    };
    bool has_trigger_first_red = false;   ///< 是否已触发第一次红板绕行
    uint8_t red_valid_count = 0;          ///< 连续检测帧数（防抖）


    cv::Rect red_roi;                     ///< 红色目标ROI

    bool is_valid_red = false;            ///< 当前帧是否检测到红色
    uint8_t red_count = 0;
    bool red_turn_flag = false;           // 正在绕行中标志。strategy_state==1 时 true。
    int center_x = -1;                    ///< 红色块中心x
    int center_y = -1;                    ///< 红色块中心y

    int w = 0;                            ///< 色块宽度
    int h = 0;                            ///< 色块高度

    bool need_turn = false;               ///< 是否需要绕行


    /* ========= 控制误差缓存 ========= */

    float DetQueue[20] = {0};             ///< 误差队列1
    int det_index = 0;
    float error1;
    float DetQueue1[20] = {0};            ///< 误差队列2
    int det_index1 = 0;

    /* ========= 图像数据 ========= */

    cv::Mat disp_color_image;             ///< 当前彩色图像
    cv::Mat disp_color_image_320;
    float turn_offset = 22;                ///< 绕行偏移量


    cv::Mat red_roi_img;
    // float category_order = 1.0f;   // 菜单类别 1~6，默认第 1 类

    bool brake = false;         // 识别到红色后开始减速
    bool has_valid_red = false;


public:

    /* ========= 功能函数 ========= */

    void detect_red_rect();               ///< 红色检测 + ROI生成

    void execute_category_strategy();     ///< 执行绕行策略

    void count_red_rect(const std::string& class_name);                ///< 里程计清零

    void run_red_classify_strategy();   ///< 红色ROI分类 + 绕行触发

private:

    ImgLineTracker& tracker_;
    ImgElementDetector& element_;
    ImgOdometer& odometer_;
    ImgCenterLine& centerline_;

};

#endif