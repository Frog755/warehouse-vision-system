# 仓库物流视觉盘点系统

> 基于激光雷达SLAM建图与自主导航的智能仓库盘点机器人

## 系统概述

本系统是一套基于激光雷达SLAM建图与自主导航的仓库物流视觉盘点机器人系统。系统集成了：
- **激光雷达建图**：基于 RPLIDAR A1/A2 进行实时环境扫描，使用 GMapping/SLAM 算法自主构建仓库地图
- **自主导航**：基于 ROS Navigation Stack，实现路径规划与动态避障
- **视觉识别**：YOLOv5s 模型自动识别货物，无需人工框选
- **库存盘点**：自动统计货物品类、数量，支持 CSV 导出

## 硬件平台

- **主控板**：龙芯 2K0300（LoongArch 架构）
- **激光雷达**：RPLIDAR A1/A2（扫描半径 12m，采样频率 8000Hz）
- **摄像头**：UVC 免驱 USB 摄像头（160x120 / 320x240）
- **电机驱动**：逐飞科技 DRV8846 电机驱动模块
- **IMU**：六轴姿态传感器（MPU6050）
- **编码器**：500线增量式编码器

## 工程目录结构

warehouse-vision-system/
├── vehicle_client/             # 小车端代码 (龙芯LoongArch板)
│   ├── lidar_driver/           # 雷达驱动模块
│   │   ├── rplidar_driver.hpp  # RPLIDAR 驱动头文件
│   │   └── rplidar_driver.cpp  # RPLIDAR 驱动实现
│   ├── slam/                   # SLAM建图模块
│   │   ├── gmapping.hpp        # GMapping 算法接口
│   │   └── gmapping.cpp        # GMapping 算法实现
│   ├── navigation/             # 自主导航模块
│   │   ├── nav_core.hpp        # 导航核心接口
│   │   └── nav_core.cpp        # 导航核心实现
│   ├── vision/                 # 视觉识别模块
│   │   ├── yolo_detector.hpp   # YOLOv5s 检测器
│   │   └── yolo_detector.cpp   # YOLOv5s 实现
│   ├── CMakeLists.txt          # CMake 交叉编译配置
│   ├── pub.cpp                 # 主程序入口
│   └── zf_driver_udp.*         # UDP 驱动
├── server/                     # PC端服务 (Ubuntu)
│   ├── app.py                  # Flask Web主程序
│   ├── tracker.py              # CSRT 跟踪算法
│   ├── udp_listener.py         # UDP 监听
│   └── requirements.txt        # Python依赖
├── frontend/                   # 浏览器前端
│   └── index.html              # 可视化页面 (含地图/导航界面)
├── line_follow/                # 巡线辅助方案
├── tcp_camera/                 # TCP图像传输方案
└── deps/                       # 依赖库文件

## 系统架构

```
┌─────────────────────────────────────────────────────────────┐
│                      浏览器前端 (Web)                        │
│   ┌───────────┐  ┌───────────┐  ┌───────────┐  ┌─────────┐ │
│   │ 实时视频  │  │ 地图显示  │  │ 导航控制  │  │ 库存统计│ │
│   └───────────┘  └───────────┘  └───────────┘  └─────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      PC端服务 (Flask)                        │
│   ┌───────────┐  ┌───────────┐  ┌───────────┐  ┌─────────┐ │
│   │ YOLO推理  │  │ SLAM建图  │  │ 导航控制  │  │ UDP监听 │ │
│   └───────────┘  └───────────┘  └───────────┘  └─────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    小车端 (龙芯2K0300)                       │
│   ┌───────────┐  ┌───────────┐  ┌───────────┐  ┌─────────┐ │
│   │ 雷达扫描  │  │ SLAM建图  │  │ 自主导航  │  │ 视觉采集│ │
│   └───────────┘  └───────────┘  └───────────┘  └─────────┘ │
└─────────────────────────────────────────────────────────────┘
```

## 核心功能

### 1. 激光雷达建图

- **雷达驱动**：支持 RPLIDAR A1/A2，360° 全方位扫描
- **SLAM算法**：基于 GMapping 的同时定位与建图
- **地图格式**：ROS OccupancyGrid 格式，支持保存/加载
- **建图精度**：±5cm（在 10m 范围内）

### 2. 自主导航

- **路径规划**：A* + DWA 混合规划算法
- **动态避障**：实时检测障碍物，自动绕行
- **目标点导航**：支持多目标点依次导航
- **自主返航**：任务完成后自动返回起点

### 3. 视觉识别

- **检测模型**：YOLOv5s（训练集 2000+ 张仓库图像）
- **识别类别**：
  - 箱体颜色：红色、蓝色、黄色、绿色、白色、黑色、棕色、灰色
  - 印刷标识：箭头朝上、防潮、易碎、易燃、防压等
  - 货物品类：电子产品、食品、服装、医疗器械等
  - 预警标签：普通、易燃易爆、易碎品
- **识别速度**：~30ms/帧（GPU）/ ~100ms/帧（CPU）
- **自动识别**：无需人工框选，系统自动检测并识别视野内所有货物

### 4. 库存盘点

- **自动统计**：实时记录识别到的货物信息
- **数据导出**：支持 CSV 格式导出盘点结果
- **预警提示**：识别到易燃易爆/易碎品时自动语音报警

## 部署步骤

### 1. PC端服务 (先启动)

```bash
cd server/
pip install -r requirements.txt
python app.py
```

启动后：
- UDP 监听：0.0.0.0:8888
- Web 服务：http://0.0.0.0:5000
- SLAM 服务：http://0.0.0.0:5001
- 导航服务：http://0.0.0.0:5002

### 2. 小车端 (后启动)

```bash
cd vehicle_client/
# 方式一: 交叉编译+部署
./build.sh

# 方式二: 手动编译
mkdir build && cd build
cmake .. && make -j16
```

部署到板子后运行：
```bash
ssh root@172.20.10.2
cd /home/root
./vehicle_client
```

### 3. 使用流程

1. 启动 PC 端服务 `python app.py`
2. 启动小车端程序 `./vehicle_client`
3. 浏览器访问 http://<PC_IP>:5000
4. 点击"开始建图"按钮，小车自动巡视仓库并构建地图
5. 建图完成后，点击"保存地图"保存当前地图
6. 切换到"导航模式"，在地图上点击目标位置
7. 小车自动规划路径并导航至目标点
8. 导航过程中自动识别货物并更新库存统计
9. 任务完成后点击"导出报表"下载 CSV 文件

## 配置项

### 雷达配置 (vehicle_client/lidar_driver/config.hpp)

```cpp
LIDAR_DEVICE    /dev/ttyUSB0    雷达串口设备
LIDAR_BAUDRATE  115200          串口波特率
SCAN_FREQUENCY  10              扫描频率 (Hz)
MAX_RANGE       12.0            最大测距范围 (m)
MIN_RANGE       0.15            最小测距范围 (m)
```

### SLAM配置 (vehicle_client/slam/config.hpp)

```cpp
MAP_RESOLUTION  0.05            地图分辨率 (m/pixel)
MAP_SIZE        2048            地图大小 (像素)
PARTICLE_NUM    30              粒子数量
UPDATE_RATE     10              更新频率 (Hz)
```

### 导航配置 (vehicle_client/navigation/config.hpp)

```cpp
MAX_SPEED       0.5             最大线速度 (m/s)
MAX_ANGULAR     1.0             最大角速度 (rad/s)
GOAL_TOLERANCE  0.2             目标点容差 (m)
OBSTACLE_DIST   0.3             障碍物安全距离 (m)
```

### YOLO配置 (vehicle_client/vision/config.hpp)

```cpp
MODEL_PATH      "models/yolov5s_warehouse.onnx"    模型路径
CONF_THRESH     0.5             置信度阈值
NMS_THRESH      0.45            NMS阈值
INPUT_SIZE      640             输入尺寸
```

## 通信协议

### UDP 数据包格式

```
┌──────────────┬────────────────────────────┐
│ 4字节长度头   │  JPEG分片数据 (≤1400字节)   │
│ uint32 BE    │                            │
└──────────────┴────────────────────────────┘
```

### 雷达数据格式

```
┌──────────┬──────────┬──────────┐
│ 角度(°)  │ 距离(mm) │ 强度     │
│ float32  │ float32  │ uint8    │
└──────────┴──────────┴──────────┘
```

### 导航指令格式

```json
{
  "type": "nav_goal",
  "x": 10.5,
  "y": 5.2,
  "theta": 1.57
}
```

## 常见问题排查

### Q1: 雷达无法连接

检查：
- 串口设备是否存在：`ls /dev/ttyUSB*`
- 是否有权限访问：`sudo chmod 666 /dev/ttyUSB0`
- 波特率是否匹配：默认 115200

### Q2: 建图效果差

方案：
- 降低移动速度，给雷达足够采样时间
- 确保环境光线适中，避免强反光表面
- 调整 PARTICLE_NUM 参数（增大可提高精度）

### Q3: 导航路径规划失败

检查：
- 地图是否完整（无大面积未知区域）
- 起点和终点是否在已知区域内
- 障碍物安全距离是否设置过大

### Q4: YOLO识别不准

方案：
- 检查模型是否正确加载
- 调整 CONF_THRESH 阈值（降低可提高召回率）
- 确保摄像头画面清晰，光照充足

### Q5: 视频流延迟高

方案：
- 降低 JPEG_QUALITY（60-75）
- 降低 TRANSMIT_FPS（5-8）
- 使用 5GHz WiFi，靠近路由器

## 技术栈

- **硬件**：龙芯 2K0300, RPLIDAR A1/A2, UVC摄像头
- **算法**：GMapping SLAM, A*+DWA 导航, YOLOv5s
- **通信**：UDP, TCP, HTTP/WebSocket
- **前端**：HTML5 Canvas, JavaScript
- **后端**：Python Flask, OpenCV, PyTorch
- **构建**：CMake, 交叉编译工具链

## 许可证

本项目仅供学习交流使用。

## 致谢

- 逐飞科技 LS2K0300 开源库
- RPLIDAR SDK
- YOLOv5 by Ultralytics
