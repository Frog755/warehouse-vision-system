//
// Created by ros on 2026/3/15.
//

#include "ImgYolo.h"
#include "ImgConfig.h"
#include "ImgCenterLine.h"
#include "ImgLineTracker.h"
#include "ImgElementDetector.h"
#include "lq_all_demo.hpp"


ImgYolo::ImgYolo(ImgLineTracker& tracker,
                 ImgElementDetector& element,
                 ImgOdometer& odometer,
                 ImgCenterLine& centerline)
    : tracker_(tracker),
      element_(element),
      odometer_(odometer),
      centerline_(centerline)
{
}

// xz：红色检测+动态ROI+模型推理+分类防抖
// 红色检测 + ROI
void ImgYolo::detect_red_rect()
{
    // region ======= 状态保护与提前退出 =======
    extern volatile bool runFlag; // 运行标志
    is_valid_red = false;

    if (red_count>= 10) {
        red_valid_count = 0;
        return;
    }

    if (strategy_state == 1) {
        red_valid_count = 0;
        return;
    }

    if (!runFlag) {
        red_valid_count = 0;
       // printf("未发车，重置红板检测状态\n");
        return;
    }

    if (element_.Zebra_crossing_flag) {
        red_valid_count = 0;
        //printf("斑马线模式，重置红板检测状态\n");
        return;
    }
    //endregion

    cv::Mat bgr_img = disp_color_image.clone();
    cv::flip(bgr_img,bgr_img,-1);
    cv::Mat hsvImage;
    cv::cvtColor(bgr_img, hsvImage, cv::COLOR_BGR2HSV);  // BGR → HSV
    int search_x = 65;//120
    int search_y = 55;//80
    int search_w = 40;//90
    int search_h = hsvImage.rows - search_y;
    search_w = std::min(search_w, hsvImage.cols - search_x);
    search_h = std::min(search_h, hsvImage.rows - search_y);
    cv::Rect search_roi(search_x, search_y, search_w, search_h);
    cv::Mat hsv_roi = hsvImage(search_roi);
    // 红色双区间HSV阈值
    // cv::Scalar lower1(0, 40, 120);   //
    // cv::Scalar upper1(12, 255, 255);
    // cv::Scalar lower2(170, 40, 120); //
    // cv::Scalar upper2(180, 255, 255);

    cv::Scalar lower1(0, 50, 130);   // 修改红色区间阈值，静态和动态不同
    cv::Scalar upper1(50, 255, 255);
    cv::Scalar lower2(170, 50, 130); //
    cv::Scalar upper2(179, 255, 255);

    cv::Mat mask1, mask2, mask;
    cv::inRange(hsv_roi, lower1, upper1, mask1);
    cv::inRange(hsv_roi, lower2, upper2, mask2);
    cv::bitwise_or(mask1, mask2, mask);

     // 形态学操作
     // 使用形态学操作去除噪声和填充小孔
      cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
     // cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);   // 开运算：去除小噪声
     // cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);  // 闭运算：填充小孔

     // 查找轮廓
     std::vector<std::vector<cv::Point>> contours;
     cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
     //printf("红色检测: 共发现%d个轮廓\n", (int)contours.size());
     has_valid_red = false;
     cv::Point center(0, 0);

     // 遍历轮廓，寻找符合条件的红色矩形
     for (const auto& cnt : contours) {
         double area = cv::contourArea(cnt);
         //printf("红色检测: 轮廓面积=%.0f\n", area);
         if (area < 2) continue;  // 面积太小，跳过
         cv::Rect rect = cv::boundingRect(cnt);

         // 宽高比过滤，红色矩形通常是长条形
         float ratio = (rect.height == 0) ? 0 : (float)rect.width / rect.height;
         //printf("红色检测: 宽高比=%.2f\n", ratio);
         if (ratio > 1.0f && ratio < 4.0f) {
             // 计算中心位置
             center.x = rect.x + rect.width / 2 + search_x;
             center.y = rect.y + rect.height / 2 + search_y;
             //printf("红色检测: 中心位置=(%d,%d)\n", center.x, center.y);
             has_valid_red = true;
             printf("有效红色！！！！！！！\n");
             if (has_valid_red) {
                 brake = true;
             }
             break;
         }
     }
    if (!has_valid_red)
    {
        return;
    }
    // 动态计算ROI尺寸: roi_size = cy * factor
    const float factor = 0.70f;//0.35
    int roi_size = static_cast<int>(center.y * factor);
    //printf("roi_size=%d\n", roi_size);
    roi_size = std::max(40, std::min(roi_size, 240));
    // 计算ROI左上角坐标
    // int roi_x = (center.x*2) - roi_size / 2;
    // int roi_y = (center.y*2) - roi_size / 2-4;//7
    float scale_x = (float)disp_color_image_320.cols / bgr_img.cols;
    float scale_y = (float)disp_color_image_320.rows / bgr_img.rows;

    // 计算ROI左上角坐标
    int roi_x = (center.x * scale_x) - roi_size / 2;
    int roi_y = (center.y * scale_y) - roi_size / 2 - 4;

    // 边界裁剪
    roi_x = std::max(0, std::min(roi_x, disp_color_image_320.cols - roi_size));
    roi_y = std::max(0, std::min(roi_y, disp_color_image_320.rows - roi_size));

    red_roi = cv::Rect(roi_x, roi_y, roi_size, roi_size);
    center_x = center.x;
    center_y = center.y;

    // 仅当y>70时才提取ROI图像用于识别//50
    if (center_y>30) {
        red_roi_img = disp_color_image_320(red_roi).clone();
        is_valid_red = true;
        // printf("截取了ROI！！！\n");
        red_valid_count++;
    } else {
        is_valid_red = false;
        red_roi_img = cv::Mat();
    }
}
void ImgYolo::count_red_rect(const std::string& class_name) {
    static int weapon_votes = 0;
    static int material_votes = 0;
    static int checked_frames = 0;
    const int vote_frames = 3;
    const int trigger_votes = 2;

    if (class_name == "Fail" || class_name == "NO") return;
    checked_frames++;
    if (class_name == "weapon")  weapon_votes++;
    if (class_name == "material") material_votes++;
    if (checked_frames < vote_frames) return;
    bool trigged = false;

    if (strategy_state == 0&& !has_trigger_first_red&& red_count < 10) {
        if (weapon_votes >= trigger_votes) {
            turn_dir = 1;
            need_turn = true;
            printf("weapon左转绕行\n");
            trigged = true;
        }else if (material_votes >= trigger_votes) {
            turn_dir = -1;
            need_turn = true;
            printf("material右转绕行\n");
            trigged = true;
        }
    }
    // ✅ 手动清零，否则下次直接触发
    weapon_votes = 0;
    material_votes = 0;
    checked_frames = 0;


    if (trigged){
        has_trigger_first_red = true;
        strategy_state = 1;
        red_turn_flag = true;
        brake = false;
        odometer_.start(2);
        odometer_.now_pulse[2] = 0.0;
        red_valid_count = 0;
        red_count++;
        has_valid_red = false;
    }

}
// 新增：绕行恢复函数
void ImgYolo::execute_category_strategy() {
    if (strategy_state !=1) {
        return;
    }
    // 读取绕行距离，控制恢复时机
    odometer_.update(2);
    const double MILEAGE_THRESHOLD =0.4;
   // printf("now2_pulse=%.3lf 米，elapsed_time=%.0lf ms\n", odometer_.now_pulse[2]);
    if (odometer_.now_pulse[2] >MILEAGE_THRESHOLD) { // 绕行0.3米后恢复
        //  printf("[RED_EVT] EXIT strategy=0 red_count=%d now2=%.3f total=%.3f\n",
        // red_count, now2_pulse, total_pulse);
        // printf("绕行完成\n");
        red_turn_flag = false;
        strategy_state = 0;
        odometer_.stop(2);
        odometer_.now_pulse[2] =0.0;
        has_trigger_first_red = false;
        red_valid_count = 0;//xz

        // 清除误差队列，避免跑偏
        centerline_.Det_True = 0.0;
        centerline_.last_Det_True = 0.0;
        memset(DetQueue, 0, sizeof(DetQueue));
        memset(DetQueue1, 0, sizeof(DetQueue1));
        det_index = 0;
        det_index1 = 0;
        turn_dir = 0;
        //printf("绕行完成：恢复默认中线\n");
    }
}

