#ifndef NAV_CORE_HPP
#define NAV_CORE_HPP

#include <cstdint>
#include <vector>
#include <functional>
#include "../slam/gmapping.hpp"

/**
 * @brief 自主导航核心模块
 * @details 基于 A* + DWA 的路径规划与避障
 */
class NavCore {
public:
    struct NavConfig {
        float max_speed = 0.5f;         // 最大线速度 (m/s)
        float max_angular = 1.0f;       // 最大角速度 (rad/s)
        float goal_tolerance = 0.2f;    // 目标点容差 (米)
        float obstacle_dist = 0.3f;     // 障碍物安全距离 (米)
        float path_resolution = 0.1f;   // 路径分辨率 (米)
        int replan_rate = 5;            // 重规划频率 (Hz)
    };

    struct Velocity {
        float linear = 0.0f;
        float angular = 0.0f;
    };

    struct GoalStatus {
        bool reached = false;
        float distance_to_goal = 0.0f;
        float estimated_time = 0.0f;
    };

    enum class NavState {
        IDLE,
        PLANNING,
        MOVING,
        OBSTACLE_AVOID,
        GOAL_REACHED,
        PLANNING_FAILED
    };

    using StatusCallback = std::function<void(NavState, const GoalStatus&)>;

    NavCore() = default;
    ~NavCore();

    /**
     * @brief 初始化导航模块
     * @param config 配置参数
     * @return 0 成功
     */
    int init(const NavConfig& config);

    /**
     * @brief 设置地图
     * @param map 占据栅格地图
     */
    void setMap(const GMapping::OccupancyGrid& map);

    /**
     * @brief 设置导航目标
     * @param goal 目标位姿
     * @return true 设置成功
     */
    bool setGoal(const GMapping::Pose& goal);

    /**
     * @brief 取消当前导航
     */
    void cancelGoal();

    /**
     * @brief 更新导航状态
     * @param current_pose 当前位姿
     * @param scan 雷达数据 (用于避障)
     * @return 速度指令
     */
    Velocity update(const GMapping::Pose& current_pose,
                    const std::vector<RPLidarDriver::ScanPoint>& scan);

    /**
     * @brief 获取当前路径
     * @return 路径点列表
     */
    std::vector<GMapping::Pose> getCurrentPath() const;

    /**
     * @brief 获取导航状态
     */
    NavState getState() const { return state_; }

    /**
     * @brief 设置状态回调
     */
    void setStatusCallback(StatusCallback callback) { status_cb_ = callback; }

private:
    NavState state_ = NavState::IDLE;
    NavConfig config_;
    GMapping::Pose goal_;
    std::vector<GMapping::Pose> current_path_;
    StatusCallback status_cb_;
};

#endif // NAV_CORE_HPP
