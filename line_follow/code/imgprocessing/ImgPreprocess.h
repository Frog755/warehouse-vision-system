//
// Created by ros on 2026/1/28.
//

#ifndef IMAGE_PREPROCESS_H
#define IMAGE_PREPROCESS_H

#include <opencv2/opencv.hpp>

/**
 * @brief 图像预处理模块
 * 职责：
 *  1. 接收原始灰度图
 *  2. 逆透视变换
 *  3. 二值化（大津 + 静态阈值）
 *  4. 输出可用于寻线的图像
 */
class ImgPreprocess
{
public:
    ImgPreprocess();

    /**
     * @brief 设置输入图像
     * @param gray 输入的原始灰度图
     */
    void setImage(const cv::Mat& gray);

    /**
     * @brief 执行完整预处理流程
     */
    void process();

    /**
     * @brief 获取二值化结果
     */
    const cv::Mat& getDaJinImage() const;

    /**
     * @brief 获取逆透视后的图像
     */
    const cv::Mat& getIPMImage() const;

private:
    /* 输入灰度图（mt9v03x） */
    cv::Mat mt9v03x_image;

    /* 中间/输出结果 */
    cv::Mat dajin_image;
    cv::Mat perspective_image;

    /* 逆透视矩阵标定 */
    cv::Mat inverseMatrix = (cv::Mat_<double>(3, 3) <<
    // -1.723529,1.997032,-110.022398,
    // -0.000000,1.169871,-190.740804,
    // -0.000000,0.025953,-3.231451);
    // -1.121511,1.650013,-104.996855,
    // -0.000000,0.500648,-83.219024
    // ,-0.000000,0.020524,-2.411614);

    -1.121511,1.650013,-98.396802,
    0.000000,0.500648,-81.216431,
    0.000000,0.020524,-2.329517);


    cv::Mat map_x, map_y;
    bool map_initialized = false;

/* 内部算法 */
    uint8_t Threshold_deal(const cv::Mat& image, uint32_t pixel_threshold);
    void Get01change_dajin(const cv::Mat& inputImage,
                           cv::Mat& outputImage,
                           int threshold_static,
                           int threshold_detach);

    cv::Mat inversePerspectiveTransform(const cv::Mat& src,
                                        const cv::Mat& matrix,
                                        int rows,
                                        int cols);

    void image_draw_rectan(cv::Mat& image);
};

#endif
