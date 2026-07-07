//
// ImgElementDetector.cpp
//

#include "ImgElementDetector.h"
#include "ImgConfig.h"
#include "ImgLineTracker.h"
#include "ImgOdometer.h"
#include <stdio.h>
#define PRINT_BLUE(text, value) printf("\x1b[34m>>> " text "\x1b[0m\n", value)

/* ==========================================================
 * 构造函数
 * ========================================================== */

ImgElementDetector::ImgElementDetector(ImgLineTracker& tracker,
                                       ImgOdometer& odometer)
    : tracker_(tracker),
      odometer_(odometer)
{

}

/* ==========================================================
 * 对外主接口
 * ========================================================== */
// TODO: 对外主接口 process
void ImgElementDetector::process(const cv::Mat& bin_image)
{
    Element_Detection(bin_image);
}


/* ==========================================================
 * 内部核心调度函数
 * ========================================================== */

void ImgElementDetector::Element_Detection(const cv::Mat& inputImage)
{
    /* 顺序可按优先级调整 */

    cross_fill(inputImage);

    LINE_JUST(1);
    LINE_JUST(2);//LEFT_just 1左连续 2右连续 3左不连续 4右不连续

    circle_fill_right(inputImage);
    circle_fill_left(inputImage);


    Staight_Detection(inputImage);

    // podao(inputImage);
    obstacle(inputImage);

    zebra_crossing(inputImage);

    /* 根据 flag 更新最终元素 */
    /* 你可以在这里写优先级判断逻辑 */
}


/* ==========================================================
 * 子检测函数
 * ========================================================== */

// TODO: 直道识别逻辑 Staight_Detection
void ImgElementDetector::Staight_Detection(const cv::Mat &inputImage)
{    /* 如果已经检测到圆环或十字，不可能是直道 */
    if (RightCirque_Flag >= 1 ||
        LeftCirque_Flag  >= 1 ||
        Cross_Flag       >= 1 ||
        Slope_flag == 1       ||               // 加
        Obstacle_Flag >= 1    )        // 加
    {
        Straight_Flag = 0;
        return;
    }

    // 创建两个点集，分别用于存储左边界和右边界上的点坐标
    std::vector<cv::Point> left_points;
    std::vector<cv::Point> right_points;

    /* 从图像底部向上取点 */
    for (int i = RESULT_ROWS - 10; i > tracker_.line_detect_center + 30; --i)
    {
        left_points.emplace_back(tracker_.l_border_[i], i);
        right_points.emplace_back(tracker_.r_border_[i], i);
    }

    // 定义一个 Lambda 函数，用于对点集进行直线拟合，并返回斜率
    auto fitLineSlope = [](const std::vector<cv::Point>& pts) -> float
    {
        // 如果点太少，返回一个极端斜率
        if (pts.size() < 2)
            return 9999.f;

        // 拟合直线，line 是 [vx, vy, x0, y0]，方向向量和通过点
        cv::Vec4f line;
        cv::fitLine(pts, line, cv::DIST_L2, 0, 0.01, 0.01);

        if (std::abs(line[0]) < 1e-6f)
            return 9999.f;

        // 返回直线的斜率 = vy / vx
        return line[1] / line[0];
    };

    // 分别对左边界和右边界拟合直线，得到斜率
    float k_left  = fitLineSlope(left_points);
    float k_right = fitLineSlope(right_points);

    // 判断两条边是否“近似垂直”：
    // 若斜率绝对值很大（例如 > 3），说明是接近垂直方向的直线
    bool is_left_vertical  = std::abs(k_left)  > 1.5f;
    bool is_right_vertical = std::abs(k_right) > 1.5f;

    /*  判断是否为直道
    *   1. 当前检测中心点是有效的（等于offline）
    *   2. 左右两边边界线都近似垂直
    */
    if (tracker_.line_detect_center <= offline + 10 && is_left_vertical && is_right_vertical)
    {
        Straight_Flag = 1;
    }
    else
    {
        Straight_Flag = 0;
    }
}

// TODO: 十字识别逻辑 cross_fill
void ImgElementDetector::cross_fill(const cv::Mat& inputImage)
{
    constexpr int LOST_THRESHOLD = 15;          // 最少左右丢线阈值
    // constexpr int VALID_THRESHOLD = 27;         // 状态5 最少左右有线数阈值
    //
    // static int l_down ; // 左下拐点的行数
    // static int l_up   ; // 左上拐点的行数
    // static int r_down ; // 右下拐点的行数
    // static int r_up   ; // 右上拐点的行数

    switch (Cross_Flag)
    {
        // region =======case 0: 确认为十字 =======
    case 0:
        if ((tracker_.lost_count <= LOST_THRESHOLD)
            // &&
            // (tracker_.r_border_lostnum <= 30 || tracker_.l_border_lostnum <= 30))
            ||
            // tracker_.valid_count <= VALID_THRESHOLD ||  // 条件放开，防止不进圆环
            RightCirque_Flag >= 1 ||
            LeftCirque_Flag  >= 1 ||
            Slope_flag == 1 ||           // 加：坡道期间不检测十字
            Obstacle_Flag >= 1        // 加：障碍物期间不检测十字
            //  || Zebra_crossing_flag == 1 // 加：斑马线期间不检测十字
            )
        {
            return;
        }
        // 初始化
        // l_down = l_up = r_down = r_up = -1;

        Cross_Flag = 1;
        // printf(">>> 检测到十字\n");
        break;
        //endregion

        // region ======= case 1:找四个拐点填充边线 =======
    case 1:
        {
            l_ok = tracker_.fit_and_correct_border(tracker_.l_border_);
            r_ok = tracker_.fit_and_correct_border(tracker_.r_border_);

            // 十字内部远端会丢线，当最长白列起始行不为截至行 说明进入十字
            if (
                tracker_.line_detect_center != offline
                // tracker_.r_border_[tracker_.r_border_begin] <= HISTOGRAM_SEARCH_RIGHT-7

                )
            {
                odometer_.start (5);
                odometer_.update(5);

                if (
                    odometer_.now_pulse[5] > 0.25 &&
                    tracker_.lost_count < 5  // 防止提前进入状态2?
                    )
                {
                    printf("r_border_ = %d \n ,r_border_begin =%d \n ",
                        tracker_.r_border_[tracker_.r_border_begin] ,tracker_.r_border_begin);

                    // 判断左右十字
                    if ( tracker_.r_border_[tracker_.r_border_begin] <= HISTOGRAM_SEARCH_RIGHT-7)
                        cross_fill_left = true;
                    else if ( tracker_.l_border_[tracker_.l_border_begin] >= HISTOGRAM_SEARCH_LEFT+7)
                        cross_fill_right = true;

                    odometer_.stop(5);
                    // Cross_Flag = 2;
                    Cross_Flag = 4;  // 跳过冗余状态

                }
            }
            break;
        }
        // endregion

        // region ======= case 2:绕行十字 =======
    case 2:
        {
            odometer_.start (5);
            odometer_.update(5);

            // 误判为十字退出
            // if (odometer_.now_pulse[5]>3)
            // {
            //     Cross_Flag=0;
            //     odometer_.stop(5);
            //     return;
            // }

            // 十字内部远端会丢线，边线起点跳到远端 说明出十字
            if (((cross_fill_left && tracker_.l_border_begin < 30) ||
                (cross_fill_right && tracker_.r_border_begin < 30))&&
                odometer_.now_pulse[5] > cross_run_odom )
            {

                if (cross_fill_left)
                    printf("cross_fill_left====");
                else if (cross_fill_right)
                    printf("cross_fill_right====");
                if (!cross_fill_left && !cross_fill_right)
                    printf("not cross_flag");

                printf("==== Cross_Flag = %d \n\n",Cross_Flag);
                odometer_.stop(5);
                Cross_Flag = 4;
            }

        if (odometer_.now_pulse[5]>3 ||
            (tracker_.l_border_continuous && tracker_.r_border_continuous && (tracker_.valid_count > 50) )) //退出
        {

            printf("now_pulse[5] = %f \n ",odometer_.now_pulse[5]);
            printf("exit cross 2");
            cross_fill_left  = false;   // 加
            cross_fill_right = false;   // 加
            Cross_Flag = 0;
            odometer_.stop(5);
        }
            break;
        }
        // endregion

        // region ======= case 3: 检测到出十字 =======
    case 3:
        {
            Cross_Flag = 4;
            // if ( (cross_fill_left && tracker_.r_border_[tracker_.r_border_begin] >= HISTOGRAM_SEARCH_RIGHT
            //     // && tracker_.r_border_[tracker_.r_border_begin + 1] >= HISTOGRAM_SEARCH_RIGHT
            //     )
            //     || (cross_fill_right && tracker_.l_border_[tracker_.l_border_begin] <= HISTOGRAM_SEARCH_LEFT) ) // 到达斜十字
            if ( // 如果有十字内侧边线较远起始点 车身就比较正&&
               (cross_fill_left &&tracker_.r_border_lostnum > 4) ||
               (cross_fill_right && tracker_.l_border_lostnum > 4))
            {
                printf("r_border_ = %d \n ,r_border_begin =%d \n ",
                    tracker_.r_border_[tracker_.r_border_begin] ,tracker_.r_border_begin);


                printf("==== Cross_Flag = %d \n\n",Cross_Flag);
                Cross_Flag = 4;
            }
            break;
        }
        // endregion

        // region ======= case 4: 准备出十字 =======
    case 4:
        {
            odometer_.start (5);
            odometer_.update(5);

            //             // ===== 调试打印：Case 4 条件检查 =====
            // printf("\n===== Cross Case 4 Debug =====\n");
            // printf("cross_fill_left  = %d, cross_fill_right = %d\n",
            //        cross_fill_left, cross_fill_right);
            //
            // // --- 左十字条件（cross_fill_left）---
            // int r_diff = tracker_.r_border_[tracker_.r_border_begin + 12]
            //            - tracker_.r_border_[tracker_.r_border_begin];
            // printf("--- Left Cross ---\n");
            // printf("r_border_[begin]     = %d\n", tracker_.r_border_[tracker_.r_border_begin]);
            // printf("r_border_[begin+12]  = %d\n", tracker_.r_border_[tracker_.r_border_begin + 12]);
            // printf("r_diff (>=40?)       = %d  [%s]\n", r_diff, r_diff >= 40 ? "PASS" : "FAIL");
            // printf("r_border_[begin] < %d ? %d [%s]\n",
            //        HISTOGRAM_SEARCH_RIGHT - 3,
            //        tracker_.r_border_[tracker_.r_border_begin],
            //        tracker_.r_border_[tracker_.r_border_begin] < HISTOGRAM_SEARCH_RIGHT - 3 ? "PASS" : "FAIL");
            // printf("r_border_[begin+12] > %d ? %d [%s]\n",
            //        HISTOGRAM_SEARCH_RIGHT + 3,
            //        tracker_.r_border_[tracker_.r_border_begin + 12],
            //        tracker_.r_border_[tracker_.r_border_begin + 12] > HISTOGRAM_SEARCH_RIGHT + 3 ? "PASS" : "FAIL");
            // printf("r_border_begin(%d) < offline+5(%d) ? [%s]\n",
            //        tracker_.r_border_begin, offline + 5,
            //        tracker_.r_border_begin < offline + 5 ? "PASS" : "FAIL");
            //
            // bool cond_left = cross_fill_left
            //               && (r_diff >= 40)
            //               && (tracker_.r_border_[tracker_.r_border_begin] < HISTOGRAM_SEARCH_RIGHT - 3)
            //               && (tracker_.r_border_[tracker_.r_border_begin + 12] > HISTOGRAM_SEARCH_RIGHT + 3)
            //               && (tracker_.r_border_begin < offline + 5);
            // printf(">>> cond_left TOTAL = %d\n", cond_left);
            //
            // // --- 右十字条件（cross_fill_right）---
            // int l_diff = tracker_.l_border_[tracker_.l_border_begin + 12]
            //            - tracker_.l_border_[tracker_.l_border_begin];
            // printf("--- Right Cross ---\n");
            // printf("l_border_[begin]     = %d\n", tracker_.l_border_[tracker_.l_border_begin]);
            // printf("l_border_[begin+12]  = %d\n", tracker_.l_border_[tracker_.l_border_begin + 12]);
            // printf("l_diff (>=40?)       = %d  [%s]\n", l_diff, l_diff >= 40 ? "PASS" : "FAIL");
            // printf("l_border_[begin] > %d ? %d [%s]\n",
            //        HISTOGRAM_SEARCH_LEFT + 3,
            //        tracker_.l_border_[tracker_.l_border_begin],
            //        tracker_.l_border_[tracker_.l_border_begin] > HISTOGRAM_SEARCH_LEFT + 3 ? "PASS" : "FAIL");
            // printf("l_border_[begin+12] > %d ? %d [%s] || < %d ? %d [%s]\n",
            //        HISTOGRAM_SEARCH_RIGHT + 3,
            //        tracker_.l_border_[tracker_.l_border_begin + 12],
            //        tracker_.l_border_[tracker_.l_border_begin + 12] > HISTOGRAM_SEARCH_RIGHT + 3 ? "PASS" : "FAIL",
            //        HISTOGRAM_SEARCH_LEFT - 3,
            //        tracker_.l_border_[tracker_.l_border_begin + 12],
            //        tracker_.l_border_[tracker_.l_border_begin + 12] < HISTOGRAM_SEARCH_LEFT - 3 ? "PASS" : "FAIL");
            // printf("l_border_begin(%d) < offline+5(%d) ? [%s]\n",
            //        tracker_.l_border_begin, offline + 5,
            //        tracker_.l_border_begin < offline + 5 ? "PASS" : "FAIL");
            //
            // bool cond_right = cross_fill_right
            //                && (l_diff >= 40)
            //                && (tracker_.l_border_[tracker_.l_border_begin] > HISTOGRAM_SEARCH_LEFT + 3)
            //                && ((tracker_.l_border_[tracker_.l_border_begin + 12] > HISTOGRAM_SEARCH_RIGHT + 3)
            //                    || (tracker_.l_border_[tracker_.l_border_begin + 12] < HISTOGRAM_SEARCH_LEFT - 3))
            //                && (tracker_.l_border_begin < offline + 5);
            // printf(">>> cond_right TOTAL = %d\n", cond_right);
            // printf("===== End Debug =====\n\n");
            // // ===== 调试打印结束 =====

            if (
                (cross_fill_left &&( tracker_.r_border_[tracker_.r_border_begin + 12] -
                tracker_.r_border_[tracker_.r_border_begin]>= 40) // 边线起始点到正确赛道上
                && tracker_.r_border_[tracker_.r_border_begin ] < HISTOGRAM_SEARCH_RIGHT -3 // 左转
                && tracker_.r_border_[tracker_.r_border_begin + 12] > HISTOGRAM_SEARCH_RIGHT +3
                && tracker_.r_border_begin < offline+15
                && !tracker_.r_border_continuous
                // && !tracker_.l_border_continuous
                // && tracker_.r_border_lostnum > 7
                ) // 防止在赛道内达到状态5
                ||
                (cross_fill_right &&( tracker_.l_border_[tracker_.l_border_begin + 12] -
                tracker_.l_border_[tracker_.l_border_begin]>= 40)
                && tracker_.l_border_[tracker_.l_border_begin ] > HISTOGRAM_SEARCH_LEFT +3  // 右转
                && (tracker_.l_border_[tracker_.l_border_begin + 12] > HISTOGRAM_SEARCH_RIGHT +3 //对应255？
                || tracker_.l_border_[tracker_.l_border_begin + 12] < HISTOGRAM_SEARCH_LEFT -3 )
                && tracker_.l_border_begin < offline+10
                // && !tracker_.r_border_continuous
                && !tracker_.l_border_continuous)
                )
            {
                printf("r_border_ = %d \n ,r_border_begin =%d \n ",
              tracker_.r_border_[tracker_.r_border_begin] ,tracker_.r_border_begin);

                printf("Cross_Flag = %d \n\n",Cross_Flag);
                Cross_Flag = 5;
                odometer_.stop(5);
            }

            if (odometer_.now_pulse[5]>4 ||
                (tracker_.l_border_continuous && tracker_.r_border_continuous && (tracker_.valid_count > 50) )) //退出
            {

                printf("now_pulse[5] = %f \n ",odometer_.now_pulse[5]);
                printf("exit cross 4");
                cross_fill_left  = false;   // 加
                cross_fill_right = false;   // 加
                Cross_Flag = 0;
                odometer_.stop(5);
            }

            break;
        }
        // endregion

        // region ======= case 5: 已经出十字，保持一段距离 =======
    case 5:
        {

            // region ==== 直接简单拉线
            if (cross_fill_right)  // 右十字
            {
                int y_top;
                if (tracker_.l_border_[tracker_.l_border_begin] > HISTOGRAM_SEARCH_LEFT - 5)
                {y_top = tracker_.l_border_begin;}          // 起点行：左边界有效起始点
                else
                {
                    y_top = RESULT_ROWS - 13;
                    // printf("y_top = %d \n ",y_top);
                }
                int y_bot = RESULT_ROWS - 3;                  // 终点行：图像高度减 3（靠近底部）
                if (y_top > 0 && y_top < y_bot )
                {
                    uint8 x_top = tracker_.l_border_[y_top];
                    uint8 x_bot = tracker_.l_border_[y_bot];
                    if (x_bot == INVALID_BORDER) x_bot = HISTOGRAM_SEARCH_LEFT;  // 右十字用左搜索边界兜底
                    // 两端均有效才线性插值，避免把 INVALID_BORDER 铺进去
                    if (x_top != INVALID_BORDER && x_bot != INVALID_BORDER)
                    {
                        float slope = (float)((int)x_bot - (int)x_top) / (float)(y_bot - y_top);

                        for (int y = y_top + 1; y < y_bot; ++y)
                        {
                            int x = (int)(x_top + slope * (y - y_top) + 0.5f);

                            // 越界保护 + 搜索范围保护（防止补到左半区去）
                            // if (x >= HISTOGRAM_SEARCH_RIGHT && x < RESULT_COLS)
                            tracker_.l_border_[y] = (uint8)x;
                        }
                    }
                }
            }
            else // 左边十字
            {
                int y_top;
                if (tracker_.r_border_[tracker_.r_border_begin] < HISTOGRAM_SEARCH_RIGHT + 5)
                {y_top = tracker_.r_border_begin;}          // 起点行：左边界有效起始点
                else
                {
                    y_top = RESULT_ROWS - 13; //这里应该是没有补线
                    // printf("y_top = %d \n ",y_top);
                }
                int y_bot = RESULT_ROWS - 3;                  // 终点行：图像高度减 3（靠近底部）
                if (y_top > 0 && y_top < y_bot )
                {
                    uint8 x_top = tracker_.r_border_[y_top];
                    uint8 x_bot = tracker_.r_border_[y_bot];
                    if (x_bot == INVALID_BORDER) x_bot = HISTOGRAM_SEARCH_RIGHT;  // 左十字用右搜索边界兜底
                    // 两端均有效才线性插值，避免把 INVALID_BORDER 铺进去
                    if (x_top != INVALID_BORDER && x_bot != INVALID_BORDER)
                    {
                        float slope = (float)((int)x_bot - (int)x_top) / (float)(y_bot - y_top);

                        for (int y = y_top + 1; y < y_bot; ++y)
                        {
                            int x = (int)(x_top + slope * (y - y_top) + 0.5f);

                            // 越界保护 + 搜索范围保护（防止补到左半区去）
                            // if (x >= HISTOGRAM_SEARCH_RIGHT && x < RESULT_COLS)
                            tracker_.r_border_[y] = (uint8)x;
                        }
                    }
                }
            }

            // endregion

            odometer_.start (5);
            odometer_.update(5);
            if (odometer_.now_pulse[5]>0.3)
            {

                // printf("odometer_.now_pulse[5] =%f \n",odometer_.now_pulse[5]);
                printf("Cross_Flag = %d \n\n",Cross_Flag);
                if (tracker_.lost_count <= LOST_THRESHOLD)
                {
                    printf("lost_count <= LOST_THRESHOLD");
                    cross_fill_left  = false;   // 清零
                    cross_fill_right = false;   // 清零
                    Cross_Flag = 0;
                    odometer_.stop(5);
                }

                else if (odometer_.now_pulse[5]>0.4) //退出
                {
                    printf(".now_pulse[5]>0.4");
                    cross_fill_left  = false;   // 清零
                    cross_fill_right = false;   // 清零
                    Cross_Flag = 0;
                    odometer_.stop(5);
                }
            }
            break;
        }
        // endregion

    default:
        break;
    }

}

void ImgElementDetector::LINE_JUST(uint8 staus)
{
    uint8 unm;
    unm=0;
    switch(staus)
    {
        case 1://左线单调判断 1连续 3不连续
        {
            for(uint8 i=valid_row_max;i>(valid_row_limit);i--)//这个i就是y的值
            {
                if (tracker_.l_border_[i] <= kLeftMaxValid &&
                    tracker_.l_border_[i] >= kLeftMinValid &&
                    tracker_.l_border_lostnum<2)
                {
                    unm++;
                }
                else if(tracker_.l_border_lostnum>=5)
                {
                    unm=0;
                    LEFT_just=3; // 左不连续
                    break;
                }
            }

            if(unm>=50)//大于多少阈值时成立
            {
                LEFT_just=1; // 左连续
            }
            else
                LEFT_just=3;
        }break;

        case 2://右线单调判定 2连续 4不连续
        {
            for(uint8 i=valid_row_max;i>(valid_row_limit);i--)
            {
                if (tracker_.r_border_[i] <= kRightMaxValid &&
                    tracker_.r_border_[i] >= kRightMinValid &&
                    tracker_.r_border_lostnum<2)
                {
                    unm++;
                }
                else if(tracker_.r_border_lostnum>=5)
                {
                    unm=0;
                    RIGHT_just=4; // 右不连续
                    break;
                }
            }

            if(unm>=50)
            {
                RIGHT_just= 2; // 右连续
            }
            else
                RIGHT_just= 4;
        }break;
    }
}

// TODO: 右圆环 大圆环 识别补线 有注释 circle_fill_right
void ImgElementDetector::circle_fill_right(const cv::Mat& inputImage)
 {

    switch (RightCirque_Flag)
    {
        // region ======= case 0 圆环识别 =======
        case 0://left边连续right不连续 右边丢线数目一定左边不丢认为是圆环
            if (
                // RIGHT_just == 4 &&
                // LEFT_just == 1  &&
                tracker_.l_border_lostnum < 7 &&
                tracker_.r_border_lostnum >=  21 &&
                // tracker_.l_border_[tracker_.l_border_begin]  < kLeftMaxValid + 3&& // 防止180弯道误识别
                // tracker_.l_border_[tracker_.l_border_begin]  > kLeftMinValid - 3&&
                tracker_.l_border_continuous &&
                tracker_.r_border_begin < offline +5  &&
                Cross_Flag == 0  &&
                ! Slope_flag &&
                Obstacle_Flag == 0 )    // 加
            {
                printf("RightCirque_Flag = %d",RightCirque_Flag);
                RightCirque_Flag=2;
                odometer_.stop(0);
            }
            break;
        // endregion

        // region ======= case 2 维持中间突变点 =======
        case 2://维持中间突变点
            odometer_.start(0);
            odometer_.update(0);
            //误判为圆环后退出圆环状态
            if (tracker_.l_border_lostnum > 7)
            {
                printf("l_border_lostnum > 7");
                RightCirque_Flag=0;
                odometer_.stop(0);
            }

            if (odometer_.now_pulse[0]>1.1)
            {
                printf("odometer_.now_pulse[0]>1.1");
                odometer_.stop(0);
                RightCirque_Flag=0;
                return;
            }
            //找弧边点 通过与上下分别隔6、12行的点进行对比以及这些点不能超过rlost_border
            for (int i=RESULT_ROWS-13;i> tracker_.r_border_begin + 15;i--)
            {
                if (
                    // tracker_.r_border_[i]    < rlost_border &&
                    // tracker_.r_border_[i+12] < rlost_border &&
                    // tracker_.r_border_[i-12] < rlost_border &&
                    // tracker_.r_border_[i-6]  < rlost_border &&
                    // tracker_.r_border_[i+6]  < rlost_border &&
                    tracker_.r_border_[i+10] >  tracker_.r_border_[i+5]  &&
                    tracker_.r_border_[i+5]  > tracker_.r_border_[i]    &&
                    tracker_.r_border_[i]    <  tracker_.r_border_[i-5]  &&
                    tracker_.r_border_[i-5]  < tracker_.r_border_[i-10] &&
                    // tracker_.r_border_[tracker_.r_border_begin] >= kRightMaxValid + 3   && // 防止提前进环
                    tracker_.r_border_begin >= (offline+10))
                {
                    //  printf("r_border_ = %d r_border_begin =%d\n",
                    //     tracker_.r_border_[tracker_.r_border_begin],tracker_.r_border_begin);
                    printf("RightCirque_Flag = %d",RightCirque_Flag);
                    RightCirque_Flag = 4;
                    odometer_.stop(0);
                    break;
                }
            }
            break;
        // endregion

        // region ======= case 3 寻找上V点 =======
        case 3://寻找上V点
            odometer_.start(0);
            odometer_.update(0);
            //误判为圆环后退出圆环状态
            if (odometer_.now_pulse[0]>2)
            {
                RightCirque_Flag=0;
                odometer_.stop(0);
                return;
            }
            //寻找上V点 通过上V点上面一段是直线来作为判断依据
            // for(int i=RESULT_ROWS-5;i>=tracker_.line_detect_center;i--)
            // {
                // if (i<85 &&
                //     tracker_.r_border_[i] >= RIGHT_BORDER_MIN &&
                //     tracker_.r_border_[i] <= RIGHT_BORDER_MAX )
                //     {
                //         num_2++;
                //     }
                //由于弧边点和V字点特征比较相似 now0_pulse需要有一定数值 让小车走过弧边点在进行V字点寻找
                if (tracker_.r_border_lostnum>=21 &&
                    // num_2 > 10 &&
                    tracker_.r_border_[tracker_.r_border_begin] <= kRightMaxValid + 1 &&
                    tracker_.r_border_[tracker_.r_border_begin] >= kRightMinValid - 1 &&
                    tracker_.r_border_begin > offline + 8 &&
                    odometer_.now_pulse[0]>= right_cirque_enter_odom)
                {
                    printf("====RightCirque_Flag = %d",RightCirque_Flag);
                    RightCirque_Flag = 4;
                    odometer_.stop(0);
                    break;
                }
            // }
            break;
        // endregion

        // region ======= case 4 进入圆环并维持一定距离 =======
        case 4://进入圆环并维持一定距离
            odometer_.start (0);
            odometer_.update(0);
            LeftCirque_Flag=0;

            if (
                 tracker_.l_border_[tracker_.l_border_begin] > HISTOGRAM_SEARCH_LEFT + 7 &&
                 tracker_.l_border_lostnum <= 10 &&
                 tracker_.l_border_begin >= offline + 12 &&
                 tracker_.r_border_begin >= offline + 20 &&
                 odometer_.now_pulse[0]>0.1)

            {
                printf("====RightCirque_Flag = %d \n\n", RightCirque_Flag);

                RightCirque_Flag=5;
                odometer_.stop(0);
            }
            break;
        // endregion

        // region ======= case 5 在圆环内部 =======
        case 5://在圆环内部
            odometer_.start(0);
            odometer_.update(0);
            LeftCirque_Flag=0;
            //误判为圆环后退出圆环状态
            if (LEFT_just==1) {
                RightCirque_Flag=0;
                odometer_.stop(0);
                return;
            }
            //右边丢线有一定的数目
            if (
                tracker_. l_border_lostnum> 19 &&
                // tracker_.l_border_[tracker_.l_border_begin] < HISTOGRAM_SEARCH_LEFT - 15 &&
                odometer_.now_pulse[0]>0.4)
            {
                printf("====RightCirque_Flag = %d \n\n", RightCirque_Flag);
                RightCirque_Flag=6;
                odometer_.stop(0);
            }
            break;
        // endregion

        // region ======= case 6: 出圆环 =======
        case 6://出圆环
            odometer_.start(0);
            odometer_.update(0);
            LeftCirque_Flag=0;
            if (LEFT_just==1) {
                RightCirque_Flag=0;
                printf("LEFT_just = ");
                odometer_.stop(0);
                return;
            }
            //左边有丢线并且找一个点判断是否为255(通过调试来看)
            if (
             // tracker_.l_border_[tracker_.l_border_begin] > HISTOGRAM_SEARCH_LEFT - 5 &&
                tracker_.l_border_lostnum < 21 &&
                // inputImage.at<uint8>(30,135)==255 &&
                // odometer_.now_pulse[0]>0.3)
            // if (tracker_.l_border_lostnum<20 &&
            //     tracker_.lost_count >= 10  &&
                 odometer_.now_pulse[0]>0.2) // 用类似于十字的检测，可能判断会慢一点，最好贴内环一点
            {
                RightCirque_Flag=7;
                odometer_.stop(0);
            }
            break;
        // endregion

        // region ======= case 7 出圆环直行需要走过一段距离防止再次进入圆环 =======
        case 7://出圆环直行需要走过一段距离防止再次进入圆环
            odometer_.start(0);
            odometer_.update(0);
            LeftCirque_Flag=0;
            if (odometer_.now_pulse[0]>1.5)
            {
                RightCirque_Flag=0;
                odometer_.stop(0);
                return;
            }
            break;
        // endregion

        default:
            break;

    }
    // region ======= 单边寻线结果处理 =======
    switch (RightCirque_Flag)
    {
        case 4://进入圆环并维持一定距离
        case 6://出圆环
            tracker_.get_right(tracker_.data_stastics_r);
            tracker_.use_right_border_only_=true;
            break;

        case 2://维持中间突变点
        case 3:
            tracker_.use_left_border_only_ =true;
            break;

        default:
            break;
    }
    // endregion
}

// TODO: 左圆环识别补线 circle_fill_left
void ImgElementDetector::circle_fill_left(const cv::Mat& inputImage)
{
    // static bool Left_Gap_Flag = false;
    // int num_2 = 0;
    //
    // constexpr int llost_border = LEFT_BORDER_MIN;

    switch (LeftCirque_Flag)
    {
        // region ======= case 0 圆环识别 =======
        case 0://right边连续left不连续 左边丢线数目一定右边不丢认为是圆环
            if (
                // RIGHT_just == 2 &&
                // LEFT_just  == 3 &&
                tracker_.r_border_lostnum < 7 &&
                // tracker_.r_border_[tracker_.r_border_begin]  < kRightMaxValid + 3&& // 防止180弯道误识别
                // tracker_.r_border_[tracker_.r_border_begin]  > kRightMinValid - 3&&
                tracker_.l_border_lostnum >= 21 &&
                // !tracker_.l_border_continuous &&
                tracker_.r_border_continuous &&
                tracker_.l_border_begin < offline + 5 &&
                Cross_Flag == 0 &&
                ! Slope_flag &&
                Obstacle_Flag == 0 )    // 加
            {
                printf("==== LeftCirque_Flag =%d\n",LeftCirque_Flag);
                LeftCirque_Flag = 2;
                odometer_.stop(1);
            }
            break;
        // endregion

        // region ======= case 2 维持中间突变点 =======
        case 2://维持中间突变点
            odometer_.start(1);
            odometer_.update(1);

            //误判为圆环后退出圆环状态
            if (tracker_.r_border_lostnum > 7)
            {
                printf("====r_border_lostnum = %d\n",tracker_.r_border_lostnum);
                LeftCirque_Flag = 0;
                odometer_.stop(1);
                return;
            }

            if (odometer_.now_pulse[1] > 1.1)
            {
                printf("====now_pulse[1] %f\n",odometer_.now_pulse[1]);

                odometer_.stop(1);
                LeftCirque_Flag = 0;
                return;
            }

            //找弧边点
            //通过与上下分别隔6、12行的点进行对比
            for (int i = RESULT_ROWS - 13; i > tracker_.l_border_begin + 15; i--)
            {
                if (
                    // tracker_.l_border_[i]     > llost_border &&
                    // tracker_.l_border_[i+12]  > llost_border &&
                    // tracker_.l_border_[i-12]  > llost_border &&
                    // tracker_.l_border_[i-6]   > llost_border &&
                    // tracker_.l_border_[i+6]   > llost_border &&
                    tracker_.l_border_[i+9]  < tracker_.l_border_[i+4] &&
                    tracker_.l_border_[i+4]   <  tracker_.l_border_[i] &&
                    tracker_.l_border_[i]     > tracker_.l_border_[i-4] &&
                    tracker_.l_border_[i-4]   >  tracker_.l_border_[i-9]  &&
                    // tracker_.l_border_[tracker_.l_border_begin] <= HISTOGRAM_SEARCH_LEFT - 3 && // 防止提前进环
                    tracker_.l_border_begin >= (offline+15)
                    )
                {

                    // printf("odometer_.total_pulse = %f\n", odometer_.total_pulse);
                    // printf("odometer_.now_pulse[1] = %f\n", odometer_.now_pulse[1]);
                    printf("====LeftCirque_Flag = %d\n", LeftCirque_Flag);

                    LeftCirque_Flag = 4;
                    odometer_.stop(1);
                    break;
                }
            }
            break;
        // endregion

        // region ======= case 3 寻找上V点 =======
        case 3://寻找上V点
            odometer_.start(1);
            odometer_.update(1);

            //误判为圆环后退出圆环状态
            if (odometer_.now_pulse[1] > 2)
            {
                LeftCirque_Flag = 0;
                odometer_.stop(1);
                return;
            }

            //寻找上V点 通过上V点上面一段是直线来作为判断依据
            for (int i = RESULT_ROWS - 15;
                 i >= tracker_.line_detect_center;
                 i--)
            {


                //由于弧边点和V字点特征比较相似
                //需要小车走过弧边点再寻找V点
                if (
                    // tracker_.l_border_lostnum >= 17 &&
                    tracker_.l_border_[tracker_.l_border_begin] <= kLeftMaxValid +1  &&
                    tracker_.l_border_[tracker_.l_border_begin] >= kLeftMinValid -1  &&
                    tracker_.l_border_begin > offline + 8 && // 现在进圆环有点慢
                    odometer_.now_pulse[1] >= left_cirque_enter_odom)  // 速度540带负压 0.25不进环 0.1进
                {
                    // printf("odometer_.total_pulse = %f\n", odometer_.total_pulse);
                    //  printf("odometer_.now_pulse[1] = %f\n", odometer_.now_pulse[1]);
                    printf("====LeftCirque_Flag = %d\n", LeftCirque_Flag);
                    LeftCirque_Flag = 4;
                    // Left_Gap_Flag   = false;
                    odometer_.stop(1);
                    break;
                }
            }
            break;
        // endregion

        // region ======= case 4 进入圆环并维持一定距离 =======
        case 4://进入圆环并维持一定距离
            odometer_.start(1);
            odometer_.update(1);
            RightCirque_Flag = 0;

            // printf("odometer_.now_pulse[1] = %f \n",odometer_.now_pulse[1]);
            if (

                tracker_.r_border_[tracker_.r_border_begin] < HISTOGRAM_SEARCH_RIGHT - 7 &&
                tracker_.r_border_lostnum <= 10 &&
                tracker_.r_border_begin >= offline + 12 &&
                tracker_.l_border_begin >= offline + 20 &&
                odometer_.now_pulse[1] > 0.3)
            {
                printf("====LeftCirque_Flag = %d \n\n", LeftCirque_Flag);

                LeftCirque_Flag = 5;
                //  printf(" LeftCirque_Flag = 5");
                odometer_.stop(1);
            } // 状态4error会大突变，方向环的d不要太大
            break;
        // endregion

        // region ======= case 5 在圆环内部 =======
        case 5://在圆环内部
            odometer_.start(1);
            odometer_.update(1);
            RightCirque_Flag = 0;
            //
            // //误判为圆环后退出圆环状态
            // if (RIGHT_just == 2)
            // {
            //     LeftCirque_Flag = 0;
            //     odometer_.stop(1);
            //     return;
            // }

       //  printf("odometer_.now_pulse[1] = %f \n",odometer_.now_pulse[1]);


            //右边丢线有一定的数目（看图像）
            if (
                tracker_.r_border_lostnum > 19 &&
                // tracker_.r_border_[tracker_.r_border_begin] > HISTOGRAM_SEARCH_RIGHT + 10 &&  // 这个会提前触发状态6
                // tracker_.r_border_begin < 50 &&
                odometer_.now_pulse[1] > 0.4)
            {
                printf("====LeftCirque_Flag = %d \n\n", LeftCirque_Flag);
                LeftCirque_Flag = 6;
                odometer_.stop(1);
            }
            break;
        // endregion

        // region ======= case 6 出圆环 =======
        case 6://出圆环
            odometer_.start(1);
            odometer_.update(1);
            RightCirque_Flag = 0;

            if (RIGHT_just == 2)
            {
                printf("====LeftCirque_Flag = %d \n\n", LeftCirque_Flag);
                LeftCirque_Flag = 0;
                odometer_.stop(1);
                return;
            }

            //右边有丢线并且判断赛道恢复
            //用类似于十字的检测
    //     if (tracker_.r_border_lostnum < 20 &&
    // tracker_.lost_count >= 5 &&
    // odometer_.now_pulse[1] > 0.25)
        // printf("odometer_.now_pulse[1] = %f \n",odometer_.now_pulse[1]);
            if (
                // tracker_.r_border_[tracker_.r_border_begin] < HISTOGRAM_SEARCH_RIGHT + 5 &&
                tracker_.r_border_lostnum < 21 &&
                // inputImage.at<uint8>(25,25)==255&&
                // tracker_.r_border_begin < 40&&
                // (inputImage.at<uint8>(52,25)==255 ||
                // inputImage.at<uint8>(25,25)==255 )&&
                odometer_.now_pulse[1] > 0.2)
            {
                printf("====LeftCirque_Flag = %d \n\n", LeftCirque_Flag);
                LeftCirque_Flag = 7;
                odometer_.stop(1);
            }
            break;
        // endregion

        // region ======= case 7 出圆环直行需要走过一段距离防止再次进入圆环 =======
        case 7:
            odometer_.start(1);
            odometer_.update(1);
            RightCirque_Flag = 0;

            if (odometer_.now_pulse[1] > 1.5)
            {
                LeftCirque_Flag = 0;
                odometer_.stop(1);
                return;
            }
            break;
        // endregion

        default:
            break;
    }

    // region ======= 单边寻线结果处理 =======
    switch (LeftCirque_Flag)
    {
        case 4:
        case 6:
            tracker_.get_left(tracker_.data_stastics_l);
            tracker_.use_left_border_only_ =true;
            break;

        case 2:
        case 3:
            tracker_.use_right_border_only_ =true;
            break;

        default:
            break;
    }
    // endregion
}

// TODO: 斑马线识别逻辑 zebra_crossing
void ImgElementDetector::zebra_crossing(const cv::Mat& inputImage)
{
    uint8 region = 0;
    const uint8 white_thresh = 20;//白色像素点数目阈值
    const uint8 change_thresh = 10;//黑白跳变数目阈值
    const uint8 min_region_rows = 5;//可以理解黑色胶带数目 这给的是最小的数目阈值
    odometer_.start(3);
    odometer_.update(3);
    for (uint8 hang = tracker_.line_detect_center; hang < RESULT_ROWS-20; hang++) // 从图像截止行到距离图像底部一定距离进行判别
    {
        uint8 row_white_count = 0;//白色像素点数目
        uint8 transitions = 0;//黑白跳变数目
        uint8 prev_pixel = inputImage.at<uchar>(hang, 60); // 起始列靠中间

        for (uint8 lie =60; lie < 100; lie++)  // 中间区域，避免边缘干扰
        {
            uint8 cur_pixel = inputImage.at<uchar>(hang, lie);//当前位置像素点
            if (cur_pixel != 0) row_white_count++;

            if ((cur_pixel != 0) != (prev_pixel != 0))//第n列和n+1列进行判断 看是否有黑白跳变
                transitions++;

            prev_pixel = cur_pixel;//依次移动
        }

        if (row_white_count > white_thresh && transitions > change_thresh)

            region++;
    }

    if (region >= min_region_rows)
    {
        Zebra_crossing_flag = true;
    }

    if (odometer_.now_pulse[3]>=0.5 && Zebra_crossing_flag == 1)
    {
        Zebra_crossing_flag = false;
        odometer_.stop(3);
    }

    // printf("Zebra_crossing_flag=%d\n",Zebra_crossing_flag);
}

// TODO: 坡道识别逻辑 podao
void ImgElementDetector::podao(const cv::Mat& inputImage)
{

    /* ========== 已在坡道状态：只更新里程计，到 1m 退出 ========== */
    if (Slope_flag)
    {
        odometer_.update(3);
        if (odometer_.now_pulse[3] >= 1.5)   // 里程计满 1 米退出
        {
            Slope_flag = false;
            odometer_.stop(3);
            printf("Slope_flag = false (odometer >= 1.0m)\n");
        }
        return;   // 坡道期间不再做新检测
    }

    /* ========== 互斥：圆环/十字期间不检测坡道 ========== */
    if (RightCirque_Flag >= 1 || LeftCirque_Flag >= 1 || Cross_Flag >= 1)
    {
        return;
    }

    /* ========== 新逻辑：连续正常宽度 + TOF 近距离 ========== */
    const int NORMAL_WIDTH_MIN = HISTOGRAM_SEARCH_RIGHT - HISTOGRAM_SEARCH_LEFT + 5;      // 正常赛道宽度下限（像素），按你的赛道标定
    const int NORMAL_WIDTH_MAX = LOST_BORDER_RIGHT - LOST_BORDER_LEFT;      // 正常赛道宽度上限（像素），按你的赛道标定
    const int TOF_THRESHOLD    = 410;     // TOF 阈值：小于 350mm 认为前方抬升
    const int REQUIRED_LINES   = 40;      // 需要连续满足条件的行数

    int normal_count = 0;

    /* 从图像底部（近处）向上扫描 */
    /* 从图像底部（近处）向上扫描 */
    for (int i = RESULT_ROWS - 1; i >= tracker_.line_detect_center; --i)
    {
        if (i < 1 || i >= RESULT_ROWS)
            continue;

        /* 任一边不连续则中断连续计数 */
        if ((!tracker_.r_border_continuous) || (!tracker_.l_border_continuous))
        {
            normal_count = 0;
            continue;
        }

        uint8_t l = tracker_.l_border_[i];
        uint8_t r = tracker_.r_border_[i];

        int track_wide = (int)r - (int)l;

        /* 判断当前行宽度是否在正常范围内 */
        if (track_wide >= NORMAL_WIDTH_MIN && track_wide <= NORMAL_WIDTH_MAX)
        {
            normal_count++;
            if (normal_count >= REQUIRED_LINES)
                break;
        }

    }

    /* 连续 40 行宽度正常 且 TOF 测距有效且小于 350mm → 判定坡道 */
    if (normal_count >= REQUIRED_LINES &&
        dl1x_distance_raw > 0 &&           // 过滤 TOF 未初始化或读失败的 0/-1
        dl1x_distance_raw < TOF_THRESHOLD)
    {
        Slope_flag = true;
        odometer_.start(3);
        printf("Slope_flag = true (TOF=%d mm, normal_lines=%d)\n",
               (int)dl1x_distance_raw, normal_count);
    }
}

// TODO: 障碍物识别逻辑 obstacle
void ImgElementDetector::obstacle(const cv::Mat& inputImage)
{
    uint8 i = 0;
    uint8 r_sort_num = 0;
    uint8 l_sort_num = 0;
    uint8 r_long_num = 0;
    uint8 l_long_num = 0;
    uint8 track_wide[RESULT_ROWS];

    switch (Obstacle_Flag)
    {
        // region ======= case 0: 障碍物检测 =======
        case 0:
        {
            /* 若当前正在处理其他元素，不检测障碍物 */
            if (LeftCirque_Flag >= 1 || RightCirque_Flag >= 1 ||
                Zebra_crossing_flag == 1 || Cross_Flag >= 1 || Slope_flag == 1)
            {
                R_Obstacle_flag = false;
                L_Obstacle_flag = false;
                break;
            }

            /* 从底部向上扫描赛道宽度 */
            for (i = RESULT_ROWS - 1; i > tracker_.line_detect_center + 20; i--)
            {
                track_wide[i] = tracker_.r_border_[i] - tracker_.l_border_[i];

                if (track_wide[i] <= 30 && track_wide[i] >= 23
                    && tracker_.l_border_begin <= 23 && tracker_.r_border_begin<=23)  // 添加左有边线连续性检测 防止弯道误判
                {
                    if (tracker_.l_border_[i] > HISTOGRAM_SEARCH_LEFT + 5) l_sort_num++;
                    if (tracker_.r_border_[i] < HISTOGRAM_SEARCH_RIGHT - 5) r_sort_num++;
                }

                if (track_wide[i] >= 33 && track_wide[i] <= 47)
                {
                    if (tracker_.l_border_[i] < HISTOGRAM_SEARCH_LEFT + 5) l_long_num++;
                    if (tracker_.r_border_[i] > HISTOGRAM_SEARCH_RIGHT - 5) r_long_num++;
                }
                // 障碍物在赛道右侧 → 后续强制使用左边界巡线
                if (r_sort_num > 30 && r_long_num > 20 &&  (!tracker_.r_border_continuous))
                {
                        R_Obstacle_flag = true;
                        L_Obstacle_flag = false;
                        Obstacle_Flag = 1;
                        printf("I = %d \n",i);
                        printf("obstacle is right\n");
                        odometer_.stop(6);
                        odometer_.start(6);

                }
                // 障碍物在赛道左侧 → 后续强制使用右边界巡线
                else if (l_sort_num > 30 && l_long_num >20 && (!tracker_.l_border_continuous))
                {
                        L_Obstacle_flag = true;
                        R_Obstacle_flag = false;
                        Obstacle_Flag = 1;
                        printf("I = %d \n",i);
                        printf("obstacle is left\n");
                        odometer_.stop(6);
                        odometer_.start(6);
                }
                // printf("l_num = %d  \n",l_num);
            }
            break;
        }
        // endregion

        // region ======= case 1: 单边绕行中（行驶 1 m） =======
        case 1:
        {
            odometer_.update(6);

            /* 优先级更高的元素触发时，强制退出障碍物状态 */
            if (LeftCirque_Flag >= 1 || RightCirque_Flag >= 1 ||
                Zebra_crossing_flag == 1 || Cross_Flag >= 1 || Slope_flag == 1)
            {
                Obstacle_Flag = 2;
                odometer_.stop(6);
                break;
            }

            /* 行驶满 1 m 后自动退出，恢复双边巡线 */
            if (odometer_.now_pulse[6] > 1.0f)
            {
                printf("obstacle run 1m, switch to bilateral\n");
                Obstacle_Flag = 2;
                odometer_.stop(6);
            }
            break;
        }
        // endregion

        // region ======= case 2: 退出障碍物状态 =======
        case 2:
        {
            R_Obstacle_flag = false;
            L_Obstacle_flag = false;
            Obstacle_Flag = 0;
            break;
        }
        // endregion

        default:
            break;
    }


}