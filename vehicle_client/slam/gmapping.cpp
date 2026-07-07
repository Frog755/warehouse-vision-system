#include "gmapping.hpp"
#include <cstdio>
#include <cmath>
#include <fstream>

GMapping::~GMapping() {
    reset();
}

int GMapping::init(const MapConfig& config) {
    config_ = config;

    // 初始化地图
    map_.width = config.map_size;
    map_.height = config.map_size;
    map_.resolution = config.resolution;
    map_.origin = {0.0f, 0.0f, 0.0f};
    map_.data.resize(map_.width * map_.height, -1);  // -1 表示未知

    // 初始化位姿
    current_pose_ = {0.0f, 0.0f, 0.0f};

    mapping_ = true;
    printf("[SLAM] GMapping initialized: %dx%d, resolution=%.2fm, particles=%d\n",
           config.map_size, config.map_size, config.resolution, config.particle_num);
    return 0;
}

void GMapping::updateScan(const std::vector<RPLidarDriver::ScanPoint>& scan, const Pose& odom_pose) {
    if (!mapping_) return;

    std::lock_guard<std::mutex> lock(mutex_);

    // TODO: 实现 GMapping 算法核心
    // 1. 粒子预测 (基于里程计)
    // 2. 计算粒子权重 (基于雷达匹配)
    // 3. 重采样
    // 4. 更新地图

    // 更新当前位姿
    current_pose_ = odom_pose;

    // TODO: 将扫描数据插入地图
    // 遍历每个扫描点，计算在地图中的位置并更新占据概率
}

GMapping::OccupancyGrid GMapping::getMap() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return map_;
}

GMapping::Pose GMapping::getCurrentPose() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return current_pose_;
}

int GMapping::saveMap(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        printf("[SLAM] Failed to save map: %s\n", filepath.c_str());
        return -1;
    }

    // 写入地图头信息
    file.write(reinterpret_cast<const char*>(&map_.width), sizeof(map_.width));
    file.write(reinterpret_cast<const char*>(&map_.height), sizeof(map_.height));
    file.write(reinterpret_cast<const char*>(&map_.resolution), sizeof(map_.resolution));
    file.write(reinterpret_cast<const char*>(&map_.origin), sizeof(map_.origin));

    // 写入地图数据
    file.write(reinterpret_cast<const char*>(map_.data.data()), map_.data.size());

    file.close();
    printf("[SLAM] Map saved: %s (%dx%d)\n", filepath.c_str(), map_.width, map_.height);
    return 0;
}

int GMapping::loadMap(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        printf("[SLAM] Failed to load map: %s\n", filepath.c_str());
        return -1;
    }

    // 读取地图头信息
    file.read(reinterpret_cast<char*>(&map_.width), sizeof(map_.width));
    file.read(reinterpret_cast<char*>(&map_.height), sizeof(map_.height));
    file.read(reinterpret_cast<char*>(&map_.resolution), sizeof(map_.resolution));
    file.read(reinterpret_cast<char*>(&map_.origin), sizeof(map_.origin));

    // 读取地图数据
    map_.data.resize(map_.width * map_.height);
    file.read(reinterpret_cast<char*>(map_.data.data()), map_.data.size());

    file.close();
    printf("[SLAM] Map loaded: %s (%dx%d)\n", filepath.c_str(), map_.width, map_.height);
    return 0;
}

void GMapping::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    mapping_ = false;
    current_pose_ = {0.0f, 0.0f, 0.0f};
    map_.data.assign(map_.data.size(), -1);
    printf("[SLAM] Map reset\n");
}
