#ifndef CAR_VISIONPIPELINE_H
#define CAR_VISIONPIPELINE_H

#include <opencv2/opencv.hpp>
#include "ImgPreprocess.h"
#include "ImgLineTracker.h"
#include "ImgElementDetector.h"
#include "ImgCenterLine.h"
#include "ImgYolo.h"

/* ================= 视觉统一调度接口 ================= */

void runVisionPipeline(
    const cv::Mat& gray,
    ImgPreprocess& preprocess,
    ImgLineTracker& lineTracker,
    ImgElementDetector& elementDetector,
    ImgCenterLine& centerLine,
    ImgYolo& yolo
);

#endif