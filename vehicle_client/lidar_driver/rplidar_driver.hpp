#ifndef RPLIDAR_DRIVER_HPP
#define RPLIDAR_DRIVER_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <functional>

/**
 * @brief RPLIDAR 激光雷达驱动
 * @details 支持 RPLIDAR A1/A2 系列，提供 360° 扫描数据
 */
class RPLidarDriver {
public:
    struct ScanPoint {
        float angle;        // 角度 (度)
        float distance;     // 距离 (米)
        uint8_t intensity;  // 信号强度
        bool start_flag;    // 扫描起始标志
    };

    struct ScanConfig {
        std::string device_path = "/dev/ttyUSB0";
        int baudrate = 115200;
        float max_range = 12.0f;      // 最大测距范围 (米)
        float min_range = 0.15f;      // 最小测距范围 (米)
        int scan_frequency = 10;      // 扫描频率 (Hz)
    };

    using ScanCallback = std::function<void(const std::vector<ScanPoint>&)>;

    RPLidarDriver() = default;
    ~RPLidarDriver();

    /**
     * @brief 初始化雷达
     * @param config 配置参数
     * @return 0 成功, <0 失败
     */
    int init(const ScanConfig& config);

    /**
     * @brief 开始扫描
     * @param callback 扫描回调函数
     * @return 0 成功, <0 失败
     */
    int startScan(ScanCallback callback);

    /**
     * @brief 停止扫描
     */
    void stopScan();

    /**
     * @brief 获取单次扫描数据
     * @param points 输出扫描点
     * @param timeout_ms 超时时间 (毫秒)
     * @return 0 成功, <0 失败
     */
    int getScanData(std::vector<ScanPoint>& points, int timeout_ms = 1000);

    /**
     * @brief 获取设备信息
     * @return 设备型号字符串
     */
    std::string getDeviceInfo();

    /**
     * @brief 检查设备是否连接
     * @return true 已连接
     */
    bool isConnected() const { return connected_; }

private:
    bool connected_ = false;
    bool scanning_ = false;
    int fd_ = -1;
    ScanConfig config_;
};

#endif // RPLIDAR_DRIVER_HPP
