  // ImgElementDetector.h

#ifndef ELEMENT_DETECTOR_H
#define ELEMENT_DETECTOR_H

#include <opencv2/opencv.hpp>
#include "ImgLineTracker.h"
#include "ImgOdometer.h"
/**
 * @brief 赛道元素枚举
 *
 * 所有外部逻辑只允许使用枚举判断，
 * 禁止直接访问内部 flag。
 */
enum class TrackElement : uint8_t {
    NONE = 0,           ///< 无特殊元素/未知
    STRAIGHT,           ///< 直道
    LEFT_CIRCLE,        ///< 左圆环
    RIGHT_CIRCLE,       ///< 右圆环
    CROSS,              ///< 十字路口
    ZEBRA,              ///< 斑马线
    OBSTACLE_LEFT,      ///< 左侧障碍
    OBSTACLE_RIGHT,     ///< 右侧障碍
    SLOPE               ///< 坡道
};

/**
 * @brief 元素检测模块
 *
 * 职责：
 * 1. 基于二值图 + 边线进行语义判断
 * 2. 维护当前赛道元素状态
 */
class ImgElementDetector {
public:
    /**
     * @brief 构造函数（依赖注入）
     */
    ImgElementDetector(ImgLineTracker& tracker,
                       ImgOdometer& odometer);

    /**
     * @brief 执行元素检测
     * @param bin_image 逆透视后的二值图（黑白图像）
     * @param valid_height 中线有效高度（图像有效区域的高度）
     */
    void process(const cv::Mat& bin_image);


    /*原始标志位（内部使用）*/
    uint8_t RightCirque_Flag = 0;   ///< 右圆环标志计数
                                    ///< case 0 圆环识别 case 2 维持中间突变点  case 3 寻找上V点  case 4 进入圆环并维持一定距离 case 5 在圆环内部 case 6: 出圆环 case 7 出圆环直行需要走过一段距离防止再次进入圆环
    uint8_t LeftCirque_Flag = 0;    ///< 左圆环标志计数
                                    ///< case 0 圆环识别 case 2 维持中间突变点  case 3 寻找上V点  case 4 进入圆环并维持一定距离 case 5 在圆环内部 case 6: 出圆环 case 7 出圆环直行需要走过一段距离防止再次进入圆环
    uint8_t Cross_Flag = 0;         ///< 十字标志计数
                                    ///<case 0: 确认为十字 case 1:找四个拐点填充边线  case 2:绕行十字 case 3: 检测到出十字 case 4: 准备出十字 case 5: 已经出十字，保持一段距离

    float left_avg  = 0.0f;         // 十字case1左边界平均数
    float right_avg = 0.0f;         // 十字case2右边界平均数


    bool cross_fill_left  = false;
    bool cross_fill_right = false;
    uint8_t Straight_Flag = 0;      ///< 直道标志计数
    bool Zebra_crossing_flag = false; ///< 斑马线标志
    bool Slope_flag = false;        ///< 坡道标志

    uint8 Obstacle_Flag = 0;        ///< 障碍物状态机 0=检测 1=单边绕行 2=退出
    bool R_Obstacle_flag = false;   ///< 右侧障碍标志
    bool L_Obstacle_flag = false;   ///< 左侧障碍标志


    uint8 LEFT_just;//1左边界连续 3左边界不连续
    uint8 RIGHT_just;//2右边界连续 4右边界不连续

private:

    /* ===== 外部依赖（构造时绑定）===== */

    ImgLineTracker& tracker_;    ///< 线跟踪器引用
    ImgOdometer& odometer_;      ///< 里程计引用

    /* ========= 成员变量 ========= */
    // 边界连续检测参数
    static constexpr uint8 kLeftMinValid  = HISTOGRAM_SEARCH_LEFT  - 3;       // 左边界有效最小值
    static constexpr uint8 kLeftMaxValid  = HISTOGRAM_SEARCH_LEFT  + 3;       // 左边界有效最大值
    static constexpr uint8 kRightMinValid = HISTOGRAM_SEARCH_RIGHT - 3;      // 右边界有效最小值
    static constexpr uint8 kRightMaxValid = HISTOGRAM_SEARCH_RIGHT + 3;     // 右边界有效最大值

    static constexpr uint8 kLostNumThreshold = 2;   // 边线丢失次数小于此值时才计入连续计数

    /* ========= 内部算法函数 ========= */

    void Element_Detection(const cv::Mat& inputImage);

    void Staight_Detection(const cv::Mat& inputImage);

    void cross_fill(const cv::Mat& inputImage);

    void LINE_JUST(uint8 staus);

    void circle_fill_right(const cv::Mat& inputImage);

    void circle_fill_left(const cv::Mat& inputImage);

    void zebra_crossing(const cv::Mat& inputImage);

    void podao(const cv::Mat& inputImage);

    void obstacle(const cv::Mat& inputImage);


    /* ========= 本帧输入缓存 ========= */

    uint8_t valid_height_ = 0;        ///< 当前帧有效高度
};

#endif // ELEMENT_DETECTOR_H

