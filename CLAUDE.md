# 仓库物流视觉盘点系统

## 项目概述

基于激光雷达 SLAM 建图与自主导航的仓库物流盘点机器人系统。
GitHub: https://github.com/Frog755/warehouse-vision-system

## 目录结构

```
vehicle_client/   # 小车端 C++ 代码（龙芯 2K0300）
  lidar_driver/   # RPLIDAR 雷达驱动
  slam/           # GMapping SLAM 建图
  navigation/     # A*+DWA 自主导航
  vision/         # YOLOv5s 自动识别
server/           # PC 端 Python Flask 服务
frontend/         # 浏览器前端（HTML/JS）
line_follow/      # 巡线辅助方案
tcp_camera/       # TCP 图像传输方案
deps/             # 依赖库（逐飞科技库、ncnn、TFLM）
```

## 关键文件

- `server/app.py` — Flask 主程序，包含所有 API
- `frontend/index.html` — 前端单页应用
- `vehicle_client/vision/yolo_detector.hpp` — YOLO 检测器接口

## API 端点

- `GET /api/categories` — 获取货物分类
- `POST /api/start_track` — 开始跟踪
- `POST /api/stop_track` — 停止跟踪
- `GET /api/status` — 获取系统状态
- `POST /api/mapping/start` — 开始建图
- `POST /api/mapping/stop` — 停止建图
- `POST /api/navigation/start` — 开始导航
- `POST /api/navigation/stop` — 停止导航
- `POST /api/yolo/detect` — YOLO 自动识别

## 运行命令

```bash
# PC 端
cd server && python app.py

# 小车端（交叉编译）
cd vehicle_client && ./build.sh
```

## 技术栈

- 硬件：龙芯 2K0300 + RPLIDAR A1/A2
- 算法：GMapping SLAM, A*+DWA, YOLOv5s
- 后端：Python Flask, OpenCV
- 通信：UDP 视频流, HTTP REST API

## 注意事项

- 雷达、SLAM、导航模块当前为接口定义，具体实现待完善
- YOLO 模型文件需单独训练并放入 `vehicle_client/vision/models/`
