//
// Created by ros on 2026/2/14.
//

#ifndef CAR_IMGCONFIG_H
#define CAR_IMGCONFIG_H

#include "zf_common_typedef.hpp"
#include "zf_common_headfile.hpp"

// static constexpr int   MAX_DUTY                = 8000 ;       // 最大 MAX_DUTY% 占空比
static constexpr float left_cirque_enter_odom  = 0 ;          // 左圆环进环时刻的里程计
static constexpr float right_cirque_enter_odom = 0 ;        // 右圆环进环时刻的里程计  //850 520 0.25 不进
static constexpr float cross_run_odom          = 0.7  ;        // 十字绕线里程计
static constexpr int   Cross_Flag_Kp           = 0   ;        // 对十字的细调
static constexpr int   Cirque_Flag_kp          = 0  ;        // 对圆环的细调

/* 图像尺寸（编译期常量） */
static constexpr int USED_ROW = 120;
static constexpr int USED_COL = 160;

static constexpr int HISTOGRAM_SEARCH_LEFT  = 59; // 直道上左边界x值是60 摄像头变化以后需调试矫正
static constexpr int HISTOGRAM_SEARCH_RIGHT = 98; // 直道上右边界x值是99 摄像头变化以后需调试矫正

static constexpr int LOST_BORDER_LEFT  = HISTOGRAM_SEARCH_LEFT  - 30; //黑白边界 左最小值 右边最大值 这两个参数主要影响转弯进入时间 通过调试看左右边界数组值来确定
static constexpr int LOST_BORDER_RIGHT = HISTOGRAM_SEARCH_RIGHT + 30;
/* 逆透视输出图像尺寸 */
static constexpr int RESULT_ROWS = 100; //y，行数,USE_H
static constexpr int RESULT_COLS = 160; //x，列数,USE_W


constexpr int INVALID_BORDER = 255;  //不合法数据，丢线

constexpr int USE_NUM = USED_ROW*9; // 八邻域最大点数，防止死循环

constexpr int COLOR_WHITE = 255;  // 白色像素的灰度值
constexpr int COLOR_GRAY  = 253;   // 防止将逆透视边线误判


constexpr int USE_H   = 100;           // 寻线有效高度

static constexpr int offline = 20;        //截至行,截至行后的数据为无效数据
static constexpr int valid_row_limit = 25;//截至行,截至行后的数据为无效数据
static constexpr int valid_row_max   = 50;//截至行,截至行后的数据为无效数据

static constexpr uint8  RIGHT_BORDER_MIN = HISTOGRAM_SEARCH_RIGHT - 10; // 右边界最小值 用于case 3
static constexpr uint8  RIGHT_BORDER_MAX = HISTOGRAM_SEARCH_RIGHT + 10; // 右边界最大值 用于case 3

static constexpr uint8  LEFT_BORDER_MIN = HISTOGRAM_SEARCH_LEFT - 10;   // 左边界最小值 用于case 3
static constexpr uint8  LEFT_BORDER_MAX = HISTOGRAM_SEARCH_LEFT + 10;   // 左边界最大值 用于case 3


extern uint8_t strategy_state;           ///< 绕行状态 0空闲 1执行
extern int turn_dir;                     ///< 绕行方向


inline volatile int16 dl1x_distance_raw = 0;          // TOF 原始距离数据（单位 mm），volatile 防优化

inline volatile bool l_ok = 0;
inline volatile bool r_ok = 0;

#endif //CAR_IMGCONFIG_H