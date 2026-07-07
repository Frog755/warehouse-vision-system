#ifndef YOLO_DETECTOR_HPP
#define YOLO_DETECTOR_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

/**
 * @brief YOLOv5s 货物检测器
 * @details 自动识别仓库货物，无需人工框选
 */
class YoloDetector {
public:
    struct Detection {
        int class_id;           // 类别ID
        std::string class_name; // 类别名称
        float confidence;       // 置信度
        cv::Rect bbox;          // 边界框
        std::string color;      // 箱体颜色
        std::string label;      // 印刷标识
        std::string category;   // 货物品类
        std::string alert_tag;  // 预警标签
    };

    struct ModelConfig {
        std::string model_path = "models/yolov5s_warehouse.onnx";
        float conf_thresh = 0.5f;       // 置信度阈值
        float nms_thresh = 0.45f;       // NMS阈值
        int input_size = 640;           // 输入尺寸
        bool use_gpu = false;           // 是否使用GPU
        int num_threads = 4;            // 线程数
    };

    // 货物类别定义
    struct CategoryMap {
        // 箱体颜色
        static constexpr const char* COLORS[] = {
            "红色", "蓝色", "黄色", "绿色", "白色", "黑色", "棕色", "灰色"
        };

        // 印刷标识
        static constexpr const char* LABELS[] = {
            "箭头朝上", "防潮", "易碎", "易燃", "防压", "此面朝上", "重心在此", "编号A"
        };

        // 货物品类
        static constexpr const char* CATEGORIES[] = {
            "电子产品", "食品", "服装", "医疗器械", "化工原料", "机械零件", "文具用品", "建材"
        };

        // 预警标签
        static constexpr const char* ALERTS[] = {
            "普通", "易燃易爆", "易碎品"
        };
    };

    YoloDetector() = default;
    ~YoloDetector();

    /**
     * @brief 初始化模型
     * @param config 模型配置
     * @return 0 成功, <0 失败
     */
    int init(const ModelConfig& config);

    /**
     * @brief 检测图像中的货物
     * @param image 输入图像
     * @param detections 输出检测结果
     * @return 检测到的数量
     */
    int detect(const cv::Mat& image, std::vector<Detection>& detections);

    /**
     * @brief 检测并标注图像
     * @param image 输入图像 (会被修改)
     * @param detections 输出检测结果
     * @return 检测到的数量
     */
    int detectAndDraw(cv::Mat& image, std::vector<Detection>& detections);

    /**
     * @brief 获取类别名称
     * @param class_id 类别ID
     * @return 类别名称
     */
    std::string getClassName(int class_id) const;

    /**
     * @brief 模型是否已加载
     */
    bool isLoaded() const { return loaded_; }

private:
    bool loaded_ = false;
    ModelConfig config_;
    cv::dnn::Net net_;

    /**
     * @brief 预处理图像
     */
    cv::Mat preprocess(const cv::Mat& image);

    /**
     * @brief 后处理推理结果
     */
    void postprocess(const cv::Mat& output, std::vector<Detection>& detections,
                     int img_width, int img_height);
};

#endif // YOLO_DETECTOR_HPP
