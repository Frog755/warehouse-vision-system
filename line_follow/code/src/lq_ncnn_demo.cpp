// #include "lq_all_demo.hpp"
// #include "lq_ncnn.h"
// #include <opencv2/opencv.hpp>
// #include <../ncnn/net.h>
// // 获取当前时间戳字符串
// // static std::string GetTimestamp()
// // {
// //     auto now = std::chrono::system_clock::now();
// //     auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
// //     time_t t = std::chrono::system_clock::to_time_t(now);
// //     tm* tm = localtime(&t);
// //
// //     std::stringstream ss;
// //     ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3) << ms.count();
// //     return ss.str();
// // }
// static LQ_NCNN g_ncnn;
// static bool g_ncnn_inited = false;
//
// void lq_ncnn_preload()
// {
//     if (!g_ncnn_inited) {
//         std::string model_param = "tiny_classifier_fp32.ncnn.param";
//         std::string model_bin   = "tiny_classifier_fp32.ncnn.bin";
//         int input_width    = 58;
//         int input_height   = 58;
//         std::vector<std::string> labels = {"material", "transport", "weapon"};
//         float mean_vals[3] = {123.675f, 116.28f, 103.53f};
//         float norm_vals[3] = {0.01712475f, 0.017507f, 0.01742919f};
//
//         g_ncnn.SetModelPath(model_param, model_bin);
//         g_ncnn.SetInputSize(input_width, input_height);
//         g_ncnn.SetLabels(labels);
//         g_ncnn.SetNormalize(mean_vals, norm_vals);
//
//         if (g_ncnn.Init()) {
//             g_ncnn_inited = true;
//             printf("NCNN模型预加载完成\n");
//             cv::Mat warmup(58, 58, CV_8UC3, cv::Scalar(128, 128, 128));
//             g_ncnn.Infer(warmup);
//             printf("NCNN推理预热完成\n");
//         } else {
//             printf("NCNN模型预加载失败！\n");
//         }
//     }
// }
// void lq_ncnn_demo(void)
// {
//     try {
//         std::cout << "========== NCNN 环境自检(无模型) ==========" << std::endl;
//
//         // 创建 ncnn::Net
//         ncnn::Net net;
//         net.opt.num_threads = 2;
//         net.opt.use_vulkan_compute = false;  // 基础检查
//
//         // 创建测试输入张量，验证 Mat 内存与基础操作
//         ncnn::Mat input(8, 8, 3); // w, h, c
//         for (int c = 0; c < input.c; ++c) {
//             float* ptr = input.channel(c);
//             for (int i = 0; i < input.w * input.h; ++i) {
//                 ptr[i] = static_cast<float>(c * 100 + i);
//             }
//         }
//
//         ncnn::Mat input_clone = input.clone();
//         float checksum = 0.0f;
//         for (int c = 0; c < input_clone.c; ++c) {
//             const float* ptr = input_clone.channel(c);
//             for (int i = 0; i < input_clone.w * input_clone.h; ++i) {
//                 checksum += ptr[i];
//             }
//         }
//
//         // 创建提取器
//         ncnn::Extractor ex = net.create_extractor();
//
//         // 未加载模型时，输入层不存在，返回非 0 属于预期行为
//         int ret = ex.input("data", input_clone);
//
//         std::cout << "Mat 维度: w=" << input_clone.w
//                   << ", h=" << input_clone.h
//                   << ", c=" << input_clone.c << std::endl;
//         std::cout << "Mat 校验和: " << checksum << std::endl;
//         std::cout << "Extractor input 返回值(无模型预期非0): " << ret << std::endl;
//
//         if (ret != 0) {
//             std::cout << "[PASS] ncnn 基础环境可用（头文件/链接/运行时对象均正常）。" << std::endl;
//             return;
//         }
//
//         std::cout << "[WARN] 未加载模型却返回0，建议进一步检查 ncnn 版本与调用逻辑。" << std::endl;
//         return;
//     } catch (const std::exception& e) {
//         std::cerr << "[FAIL] ncnn 环境自检异常: " << e.what() << std::endl;
//         return;
//     } catch (...) {
//         std::cerr << "[FAIL] ncnn 环境自检出现未知异常" << std::endl;
//         return;
//     }
// }
//
// // int lq_ncnn_classify(cv::Mat roi_img)
// // {
// //     static LQ_NCNN ncnn;
// //     static bool inited = false;
// //
// //     if (!inited) {
// //         std::string model_param = "tiny_classifier_fp32.ncnn.param";
// //         std::string model_bin   = "tiny_classifier_fp32.ncnn.bin";
// //         int input_w = 58;
// //         int input_h = 58;
// //         std::vector<std::string> labels = {"material","transport","weapon"};
// //         float mean_vals[3] = {123.675f, 116.28f, 103.53f};
// //         float norm_vals[3] = {0.01712475f, 0.017507f, 0.01742919f};
// //
// //         ncnn.SetModelPath(model_param, model_bin);
// //         ncnn.SetInputSize(input_w, input_h);
// //         ncnn.SetLabels(labels);
// //         ncnn.SetNormalize(mean_vals, norm_vals);
// //         ncnn.Init();
// //         inited = true;
// //     }
// //
// //     if (roi_img.empty() || roi_img.channels() != 3)
// //         return -1;
// //
// //     std::string res = ncnn.Infer(roi_img);
// //
// //     if (res == "material") return 0;
// //     if (res == "transport") return 1;
// //     if (res == "weapon") return 2;
// //
// //     return -1;
// // }
//
//  std::string lq_ncnn_photo_demo(cv::Mat color_frame)
// {
//     // printf("========================================\n");
//     // printf("       NCNN 图像分类推理示例\n");
//     // printf("========================================\n\n");
//
//     // ==================== 配置区域 ====================
//     // 测试图片路径（修改为你的图片路径）
//    // std::string test_image_path = "test.jpg";
//     // 检查输入是否为有效的彩色图
//     if (color_frame.empty() || color_frame.channels() != 3) {
//         //printf("不是彩色图! 通道数=%d, 是否为空=%d\n",
//                //color_frame.empty() ? -1 : color_frame.channels(), color_frame.empty());
//         return "NO";
//     }
//    // 模型配置
//     // static LQ_NCNN ncnn;
//     // static bool inited = false;
//     // if (!inited) {
//     //     std::string model_param = "tiny_classifier_fp32.ncnn.param";
//     //     std::string model_bin   = "tiny_classifier_fp32.ncnn.bin";
//     //     int input_width    = 58;
//     //     int input_height   = 58;
//     //
//     //     // 类别标签（顺序必须与训练时一致）
//     //     std::vector<std::string> labels = {"material", "transport", "weapon"};
//     //
//     //     // 归一化参数（ImageNet标准）
//     //     float mean_vals[3] = {123.675f, 116.28f, 103.53f};
//     //     float norm_vals[3] = {0.01712475f, 0.017507f, 0.01742919f};
//     //     // =================================================
//     //
//     //     // 创建NCNN对象并配置
//     //     //LQ_NCNN ncnn;
//     //     ncnn.SetModelPath(model_param, model_bin);
//     //     ncnn.SetInputSize(input_width, input_height);
//     //     ncnn.SetLabels(labels);
//     //     ncnn.SetNormalize(mean_vals, norm_vals);
//     //
//     //     // 初始化模型
//     //     //printf("[%s] 正在加载模型...\n", GetTimestamp().c_str());
//     //     if (!ncnn.Init()) {
//     //         //printf("[%s] 模型加载失败!\n", GetTimestamp().c_str());
//     //         return "Fail";
//     //     }
//     //     inited = true;
//     // }
//    // printf("[%s] 模型加载成功!\n\n", GetTimestamp().c_str());
//     lq_ncnn_preload();
//     // 读取测试图片
//    // printf("[%s] 输入尺寸: %d * %d\n", GetTimestamp().c_str(),input_width,input_height);
//     //printf("[%s] 彩色图像尺寸: %d x %d\n\n", GetTimestamp().c_str(), color_frame.cols, color_frame.rows);
//
//     // 注意: OpenCV读取的是BGR格式，但在推理时会自动转换为RGB格式以匹配训练时的输入
//     // 训练使用PIL读取的RGB格式，因此需要色彩空间转换
//
//     // 推理
//     //printf("[%s] 开始推理...\n", GetTimestamp().c_str());
//     auto start = std::chrono::high_resolution_clock::now();
//     std::string result = g_ncnn.Infer(color_frame);
//     auto end = std::chrono::high_resolution_clock::now();
//     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//
//     // 输出结果
//    // printf("\n========================================\n");
//     //printf("推理结果: %s\n", result.c_str());
//     //printf("推理耗时: %ld ms\n", duration.count());
//     //printf("========================================\n");
//
//     return result;
// }
