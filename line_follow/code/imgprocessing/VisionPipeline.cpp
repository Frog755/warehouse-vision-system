#include "VisionPipeline.h"
#include "ImgConfig.h"


using namespace cv;

void runVisionPipeline(
    const cv::Mat& gray,
    ImgPreprocess& preprocess,
    ImgLineTracker& lineTracker,
    ImgElementDetector& elementDetector,
    ImgCenterLine& centerLine,
    ImgYolo& yolo
)
{
    /* ========== 1. 预处理 ========== */
    preprocess.setImage(gray);
    preprocess.process();

     // yolo.detect_red_rect();    // 不调识别代码的时候这里要注释，yolo注释5,还有一处在main.cpp

    cv::Mat dajin = preprocess.getDaJinImage();
    Mat ipm   = preprocess.getIPMImage();  // 逆透视图像

    /* ========== 2. 寻线 ========== */
    lineTracker.process(ipm); // 寻线，最长白列


    /* ========== 4. 元素检测 ========== */
    elementDetector.process(ipm);

    /* ========== 5. 中线计算 ========== */
    centerLine.process();
}
