#include "yolo_detector.hpp"
#include <cstdio>
#include <algorithm>

// 类别名称定义
namespace {
    const std::vector<std::string> CLASS_NAMES = {
        "red_box", "blue_box", "yellow_box", "green_box",
        "white_box", "black_box", "brown_box", "gray_box",
        "arrow_up", "moisture_proof", "fragile", "flammable",
        "no_press", "this_side_up", "center_here", "code_a",
        "electronics", "food", "clothing", "medical",
        "chemical", "machinery", "stationery", "building_material",
        "normal", "explosive", "fragile_item"
    };

    const std::vector<std::string> COLOR_NAMES = {
        "红色", "蓝色", "黄色", "绿色", "白色", "黑色", "棕色", "灰色"
    };

    const std::vector<std::string> LABEL_NAMES = {
        "箭头朝上", "防潮", "易碎", "易燃", "防压", "此面朝上", "重心在此", "编号A"
    };

    const std::vector<std::string> CATEGORY_NAMES = {
        "电子产品", "食品", "服装", "医疗器械", "化工原料", "机械零件", "文具用品", "建材"
    };

    const std::vector<std::string> ALERT_NAMES = {
        "普通", "易燃易爆", "易碎品"
    };
}

YoloDetector::~YoloDetector() {
    // 释放模型资源
}

int YoloDetector::init(const ModelConfig& config) {
    config_ = config;

    // 加载 ONNX 模型
    try {
        net_ = cv::dnn::readNetFromONNX(config.model_path);

        if (config.use_gpu) {
            net_.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
            net_.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        } else {
            net_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
            net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
            net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        }

        loaded_ = true;
        printf("[YOLO] Model loaded: %s (input_size=%d, conf=%.2f, nms=%.2f)\n",
               config.model_path.c_str(), config.input_size,
               config.conf_thresh, config.nms_thresh);
        return 0;
    } catch (const cv::Exception& e) {
        printf("[YOLO] Failed to load model: %s\n", e.what());
        return -1;
    }
}

int YoloDetector::detect(const cv::Mat& image, std::vector<Detection>& detections) {
    if (!loaded_ || image.empty()) {
        return 0;
    }

    detections.clear();

    // 预处理
    cv::Mat blob = preprocess(image);

    // 推理
    net_.setInput(blob);
    cv::Mat output = net_.forward();

    // 后处理
    postprocess(output, detections, image.cols, image.rows);

    return static_cast<int>(detections.size());
}

int YoloDetector::detectAndDraw(cv::Mat& image, std::vector<Detection>& detections) {
    int count = detect(image, detections);

    // 绘制检测结果
    for (const auto& det : detections) {
        // 绘制边界框
        cv::Scalar color(0, 255, 0);  // 绿色
        cv::rectangle(image, det.bbox, color, 2);

        // 绘制标签
        std::string label = det.class_name + " " + cv::format("%.0f%%", det.confidence * 100);
        int baseline = 0;
        cv::Size label_size = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
        cv::rectangle(image,
                     cv::Point(det.bbox.x, det.bbox.y - label_size.height - 5),
                     cv::Point(det.bbox.x + label_size.width, det.bbox.y),
                     color, -1);
        cv::putText(image, label,
                   cv::Point(det.bbox.x, det.bbox.y - 5),
                   cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }

    return count;
}

std::string YoloDetector::getClassName(int class_id) const {
    if (class_id >= 0 && class_id < static_cast<int>(CLASS_NAMES.size())) {
        return CLASS_NAMES[class_id];
    }
    return "unknown";
}

cv::Mat YoloDetector::preprocess(const cv::Mat& image) {
    cv::Mat blob;
    cv::dnn::blobFromImage(image, blob, 1.0 / 255.0,
                           cv::Size(config_.input_size, config_.input_size),
                           cv::Scalar(0, 0, 0), true, false);
    return blob;
}

void YoloDetector::postprocess(const cv::Mat& output, std::vector<Detection>& detections,
                                int img_width, int img_height) {
    // TODO: 实现 YOLOv5 后处理
    // 1. 解析输出张量
    // 2. 过滤低置信度检测
    // 3. NMS 非极大值抑制
    // 4. 坐标映射回原图

    // 占位实现：返回空结果
    (void)output;
    (void)detections;
    (void)img_width;
    (void)img_height;
}
