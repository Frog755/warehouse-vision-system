#include "nav_core.hpp"
#include <cstdio>
#include <cmath>
#include <algorithm>

NavCore::~NavCore() {
    cancelGoal();
}

int NavCore::init(const NavConfig& config) {
    config_ = config;
    state_ = NavState::IDLE;
    printf("[NAV] Navigation initialized: max_speed=%.2f m/s, max_angular=%.2f rad/s\n",
           config.max_speed, config.max_angular);
    return 0;
}

void NavCore::setMap(const GMapping::OccupancyGrid& map) {
    // TODO: 将地图转换为内部代价地图格式
    printf("[NAV] Map set: %dx%d, resolution=%.2f\n",
           map.width, map.height, map.resolution);
}

bool NavCore::setGoal(const GMapping::Pose& goal) {
    goal_ = goal;
    state_ = NavState::PLANNING;

    printf("[NAV] Goal set: (%.2f, %.2f, %.2f)\n",
           goal.x, goal.y, goal.theta);

    // TODO: 调用 A* 算法规划路径
    // current_path_ = astar_planner_.plan(current_pose_, goal_, map_);

    if (current_path_.empty()) {
        state_ = NavState::PLANNING_FAILED;
        printf("[NAV] Path planning failed\n");
        return false;
    }

    state_ = NavState::MOVING;
    printf("[NAV] Path planned with %zu waypoints\n", current_path_.size());
    return true;
}

void NavCore::cancelGoal() {
    state_ = NavState::IDLE;
    current_path_.clear();
    printf("[NAV] Goal cancelled\n");
}

NavCore::Velocity NavCore::update(const GMapping::Pose& current_pose,
                                   const std::vector<RPLidarDriver::ScanPoint>& scan) {
    Velocity vel = {0.0f, 0.0f};

    if (state_ != NavState::MOVING && state_ != NavState::OBSTACLE_AVOID) {
        return vel;
    }

    // TODO: 实现 DWA 局部规划器
    // 1. 检测前方障碍物
    // 2. 如果有障碍物，切换到避障模式
    // 3. 计算最优速度指令

    // 检查是否到达目标
    float dx = goal_.x - current_pose.x;
    float dy = goal_.y - current_pose.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < config_.goal_tolerance) {
        state_ = NavState::GOAL_REACHED;
        printf("[NAV] Goal reached\n");
        if (status_cb_) {
            status_cb_(state_, {true, 0.0f, 0.0f});
        }
        return vel;
    }

    // 简单的朝向目标前进 (占位实现)
    float target_angle = std::atan2(dy, dx);
    float angle_diff = target_angle - current_pose.theta;

    // 角度归一化到 [-pi, pi]
    while (angle_diff > M_PI) angle_diff -= 2 * M_PI;
    while (angle_diff < -M_PI) angle_diff += 2 * M_PI;

    // 设置速度
    vel.angular = std::clamp(angle_diff * 2.0f, -config_.max_angular, config_.max_angular);
    vel.linear = config_.max_speed * std::max(0.0f, 1.0f - std::abs(angle_diff) / M_PI);

    return vel;
}

std::vector<GMapping::Pose> NavCore::getCurrentPath() const {
    return current_path_;
}
