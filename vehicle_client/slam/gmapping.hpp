#ifndef GMAPPING_HPP
#define GMAPPING_HPP

#include <cstdint>
#include <vector>
#include <mutex>
#include "../lidar_driver/rplidar_driver.hpp"

/**
 * @brief GMapping SLAM 建图算法接口
 * @details 基于粒子滤波的实时建图与定位
 */
class GMapping {
public:
    struct MapConfig {
        float resolution = 0.05f;       // 地图分辨率 (米/像素)
        int map_size = 2048;            // 地图大小 (像素)
        int particle_num = 30;          // 粒子数量
        int update_rate = 10;           // 更新频率 (Hz)
        float xmin = -50.0f;            // 地图X最小值
        float xmax = 50.0f;             // 地图X最大值
        float ymin = -50.0f;            // 地图Y最小值
        float ymax = 50.0f;             // 地图Y最大值
    };

    struct Pose {
        float x = 0.0f;
        float y = 0.0f;
        float theta = 0.0f;
    };

    struct OccupancyGrid {
        int width;
        int height;
        float resolution;
        Pose origin;
        std::vector<int8_t> data;  // 0-100: 占据概率, -1: 未知
    };

    GMapping() = default;
    ~GMapping();

    /**
     * @brief 初始化 SLAM
     * @param config 配置参数
     * @return 0 成功, <0 失败
     */
    int init(const MapConfig& config);

    /**
     * @brief 更新雷达数据
     * @param scan 雷达扫描数据
     * @param odom_pose 里程计位姿
     */
    void updateScan(const std::vector<RPLidarDriver::ScanPoint>& scan, const Pose& odom_pose);

    /**
     * @brief 获取当前地图
     * @return 占据栅格地图
     */
    OccupancyGrid getMap() const;

    /**
     * @brief 获取当前位姿估计
     * @return 位姿
     */
    Pose getCurrentPose() const;

    /**
     * @brief 保存地图到文件
     * @param filepath 文件路径
     * @return 0 成功
     */
    int saveMap(const std::string& filepath);

    /**
     * @brief 加载地图文件
     * @param filepath 文件路径
     * @return 0 成功
     */
    int loadMap(const std::string& filepath);

    /**
     * @brief 重置建图
     */
    void reset();

    /**
     * @brief 检查是否正在建图
     */
    bool isMapping() const { return mapping_; }

private:
    bool mapping_ = false;
    MapConfig config_;
    Pose current_pose_;
    OccupancyGrid map_;
    mutable std::mutex mutex_;
};

#endif // GMAPPING_HPP
