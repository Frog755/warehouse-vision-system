#include "rplidar_driver.hpp"
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// RPLIDAR 协议常量
namespace {
    constexpr uint8_t SYNC_BYTE = 0xA5;
    constexpr uint8_t GET_INFO_BYTE = 0x50;
    constexpr uint8_t START_SCAN_BYTE = 0x20;
    constexpr uint8_t STOP_BYTE = 0x25;
}

RPLidarDriver::~RPLidarDriver() {
    stopScan();
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

int RPLidarDriver::init(const ScanConfig& config) {
    config_ = config;

    // 打开串口设备
    fd_ = ::open(config.device_path.c_str(), O_RDWR | O_NOCTTY);
    if (fd_ < 0) {
        printf("[LIDAR] Failed to open device: %s\n", config.device_path.c_str());
        return -1;
    }

    // 配置串口参数
    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(fd_, &tty) != 0) {
        printf("[LIDAR] Failed to get terminal attributes\n");
        ::close(fd_);
        fd_ = -1;
        return -2;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;

    if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
        printf("[LIDAR] Failed to set terminal attributes\n");
        ::close(fd_);
        fd_ = -1;
        return -3;
    }

    connected_ = true;
    printf("[LIDAR] Device initialized: %s @ %d baud\n",
           config.device_path.c_str(), config.baudrate);
    return 0;
}

int RPLidarDriver::startScan(ScanCallback callback) {
    if (!connected_) {
        printf("[LIDAR] Device not connected\n");
        return -1;
    }

    // TODO: 发送开始扫描命令
    // uint8_t cmd[] = {SYNC_BYTE, START_SCAN_BYTE};
    // write(fd_, cmd, sizeof(cmd));

    scanning_ = true;
    printf("[LIDAR] Scan started\n");
    return 0;
}

void RPLidarDriver::stopScan() {
    if (scanning_) {
        // TODO: 发送停止命令
        // uint8_t cmd[] = {SYNC_BYTE, STOP_BYTE};
        // write(fd_, cmd, sizeof(cmd));
        scanning_ = false;
        printf("[LIDAR] Scan stopped\n");
    }
}

int RPLidarDriver::getScanData(std::vector<ScanPoint>& points, int timeout_ms) {
    if (!connected_ || !scanning_) {
        return -1;
    }

    // TODO: 从串口读取扫描数据
    // 实际实现需要解析 RPLIDAR 协议数据包
    points.clear();

    // 模拟数据 (开发调试用)
    for (int i = 0; i < 360; i++) {
        ScanPoint point;
        point.angle = static_cast<float>(i);
        point.distance = 1.0f + 0.5f * sin(i * M_PI / 180.0f);
        point.intensity = 100;
        point.start_flag = (i == 0);
        points.push_back(point);
    }

    return 0;
}

std::string RPLidarDriver::getDeviceInfo() {
    if (!connected_) {
        return "Not connected";
    }

    // TODO: 发送获取设备信息命令并解析响应
    return "RPLIDAR A1 (Firmware: 1.29)";
}
