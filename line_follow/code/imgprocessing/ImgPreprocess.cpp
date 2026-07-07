//
// Created by ros on 2026/1/28.
//

#include "ImgPreprocess.h"
#include <stdexcept>
#include "ImgConfig.h"

using namespace cv;

// 构造函数
ImgPreprocess::ImgPreprocess()
{

}

// TODO: 设置输入图像
void ImgPreprocess::setImage(const cv::Mat& inputImage)
{
    if (inputImage.rows != USED_ROW ||
        inputImage.cols != USED_COL ||
        inputImage.type() != CV_8UC1)// 检查输入图像大小和类型为灰度图像
    {
        throw std::invalid_argument("Input image size or type does not match mt9v03x_image");
    }

    inputImage.copyTo(mt9v03x_image);
}

// TODO: 预处理主流程
void ImgPreprocess::process()
{
    if (mt9v03x_image.empty())
    {
        throw std::runtime_error("Source image not set");
    }

    /* ---------- 1. 摄像头方向修正 ----------
     * MT9V03X 摄像头反装，因此对原始图像进行翻转
     */
    cv::Mat corrected;
    cv::flip(mt9v03x_image, corrected, -1);
    // 调试看图像用，反转两次（直接在mian函数处理）
    cv::flip(corrected, corrected, -1);
    /* ---------- 2. 二值化处理 ----------
     * 使用大津法确定自适应阈值，
     * 并结合静态阈值保证在不同光照下的稳定性
     */
    dajin_image.create(corrected.size(), CV_8UC1);

    // 大津法计算阈值
    // constexpr int threshold_static = 125; //光线比较暗的话调大第一个阈值 光线比较亮调小第一个阈值
    constexpr int threshold_static = 130; //中午  // 调大第一个阈值 白色变少
    constexpr int threshold_detach = 220; //第二个参数可以基本不变

    Get01change_dajin(
        corrected,
        dajin_image,
        threshold_static,
        threshold_detach
    );

    /* ---------- 3. 逆透视变换 ----------
     * 将赛道从摄像头视角映射到俯视视角，
     * 便于后续几何分析和边界搜索
     */
    perspective_image = inversePerspectiveTransform(
        dajin_image,
        inverseMatrix,
        RESULT_ROWS,
        RESULT_COLS
    );

    /* ---------- 4. 调试绘制 ----------
     */
    image_draw_rectan(perspective_image);
}

/* =========================
 * Getter 接口，获取对象内部状态
 * ========================= */
const Mat& ImgPreprocess::getDaJinImage() const
{
    return dajin_image;
}

const Mat& ImgPreprocess::getIPMImage() const
{
    return perspective_image;
}

// 内部算法
/**
 * @brief 阈值统计/处理（工具函数）
 */
uint8_t ImgPreprocess::Threshold_deal(
    const Mat& image,
    uint32_t pixel_threshold)
{
    CV_Assert(!image.empty());// 图像不能为空
    CV_Assert(image.type() == CV_8UC1);

    const int GrayScale = 256; // 灰度级范围
    int pixelCount[GrayScale] = {0}; // 灰度值统计
    float pixelPro[GrayScale] = {0.0f}; // 灰度值比例

    const int width  = image.cols; // 图像宽度
    const int height = image.rows; // 图像高度
    const int pixelSum = width * height; // 总像素数

    // 统计灰度值分布
    uint32_t gray_sum = 0; // 灰度值总和
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            uint8_t pixelValue = image.at<uint8_t>(i, j);
            pixelCount[pixelValue]++;
            gray_sum += pixelValue;
        }
    }

    // 概率分布
    for (int i = 0; i < GrayScale; i++)
        pixelPro[i] = static_cast<float>(pixelCount[i]) / pixelSum;

    float w0 = 0.0f, w1 = 0.0f;
    float u0tmp = 0.0f, u1tmp = 0.0f;
    float u0 = 0.0f, u1 = 0.0f, u = 0.0f;
    float deltaTmp = 0.0f, deltaMax = 0.0f;

    uint8_t threshold = 0;

    // 限制搜索范围
    for (uint32_t t = 0; t < pixel_threshold; t++)
    {
        w0 += pixelPro[t];
        u0tmp += t * pixelPro[t];

        w1 = 1.0f - w0;
        if (w0 == 0.0f || w1 == 0.0f)
            continue;

        u1tmp = static_cast<float>(gray_sum) / pixelSum - u0tmp;

        if (w0 > 0.0f) u0 = u0tmp / w0; // 背景平均灰度
        if (w1 > 0.0f) u1 = u1tmp / w1; // 前景平均灰度
        u  = u0tmp + u1tmp;

        deltaTmp = w0 * (u0 - u) * (u0 - u)
                 + w1 * (u1 - u) * (u1 - u);

        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = t;
        }
    }

    return threshold;
}


/**
 * @brief 大津 + 静态阈值二值化
 */
void ImgPreprocess::Get01change_dajin(
    const Mat& inputImage,
    Mat& outputImage,
    int threshold_static,
    int threshold_detach)
{
    CV_Assert(!inputImage.empty());
    CV_Assert(inputImage.type() == CV_8UC1);

    outputImage = Mat::zeros(inputImage.size(), CV_8UC1);

    // 动态阈值（工程 Otsu）
    int dynamicThreshold = Threshold_deal(inputImage, threshold_detach);
    if (dynamicThreshold < threshold_static)
        dynamicThreshold = threshold_static;

    for (int i = 0; i < inputImage.rows; i++)
    {
        const uint8_t* src = inputImage.ptr<uint8_t>(i);
        uint8_t* dst = outputImage.ptr<uint8_t>(i);

        for (int j = 0; j < inputImage.cols; j++)
        {
            int thre = dynamicThreshold;

            // 列补偿（你原来的经验规则）
            if (j <= 15 || j >= 65 ||
               (j > 70 && j <= 75))
            {
                thre -= 10;
            }

            dst[j] = (src[j] >= thre) ? 255 : 0;
        }
    }
}

/**
* @brief 逆透视：对给定边线的每个点 (x, y)，应用 3x3 矩阵，返回原图坐标的 x 值到 transformed_x 数组中
* border:       原始边线，按 y 存储 x 坐标（大小为 use_h）
* transformed_x: 返回变换后的 x 坐标（大小为 use_h）
* use_h:        图像高度（行数）
* inverseMat:   3x3 的逆透视矩阵
 */
cv::Mat ImgPreprocess::inversePerspectiveTransform(const cv::Mat& src, const cv::Mat& inverseMatrix, int result_rows, int result_cols){
    CV_Assert(src.channels() == 1);
    CV_Assert(inverseMatrix.rows == 3 && inverseMatrix.cols == 3 && inverseMatrix.type() == CV_64F);

    if (!map_initialized) {
        // 构造目标图像的齐次坐标矩阵
        cv::Mat dst_homo(3, result_rows * result_cols, CV_64F);
        for (int y = 0; y < result_rows; ++y) {
            for (int x = 0; x < result_cols; ++x) {
                int idx = y * result_cols + x;
                dst_homo.at<double>(0, idx) = x;
                dst_homo.at<double>(1, idx) = y;
                dst_homo.at<double>(2, idx) = 1.0;
            }
        }

        // 逆透视映射
        cv::Mat mapped = inverseMatrix * dst_homo;

        // 归一化
        for (int i = 0; i < mapped.cols; ++i) {
            double w = mapped.at<double>(2, i);
            if (w != 0.0) {
                mapped.at<double>(0, i) /= w;
                mapped.at<double>(1, i) /= w;
            }
        }

        // 初始化映射图 map_x 和 map_y
        map_x.create(result_rows, result_cols, CV_32F);
        map_y.create(result_rows, result_cols, CV_32F);
        for (int i = 0; i < result_rows; ++i) {
            for (int j = 0; j < result_cols; ++j) {
                int idx = i * result_cols + j;
                map_x.at<float>(i, j) = static_cast<float>(mapped.at<double>(0, idx));
                map_y.at<float>(i, j) = static_cast<float>(mapped.at<double>(1, idx));
            }
        }

        map_initialized = true;
    }

    // 直接使用缓存的 map 进行 remap
    cv::Mat dst;
    cv::remap(src, dst, map_x, map_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT, 254);
    return dst;
}

/**
 * @brief 调试用矩形绘制一个黑框
 */
void ImgPreprocess::image_draw_rectan(cv::Mat& image) {
    CV_Assert(!image.empty());// 图像不能为空
    CV_Assert(image.type() == CV_8UC1);

    const int get_image_h = image.rows; // 图像高度
    const int get_image_w = image.cols; // 图像宽度

    // 绘制左右两列的黑框
    for (int i = 0; i < get_image_h; i++) {
        image.at<uchar>(i, 0) = 0;       // 第一列
        image.at<uchar>(i, 1) = 0;       // 第二列
        image.at<uchar>(i, get_image_w - 1) = 0; // 最后一列
        image.at<uchar>(i, get_image_w - 2) = 0; // 倒数第二列
    }

    // 绘制上下两行的黑框
    for (int i = 0; i < get_image_w; i++) {
        image.at<uchar>(0, i) = 0;       // 第一行
        image.at<uchar>(1, i) = 0;       // 第二行
        // image.at<uchar>(image_h - 1, i) = 0; // 最后一行（如果需要，可以解注释）
    }
}


