#include "ImgCenterLine.h"
#include "ImgLineTracker.h"
#include "ImgElementDetector.h"
#include <algorithm>

/* ================= 构造 ================= */

ImgCenterLine::ImgCenterLine(ImgLineTracker& tracker,
                             ImgElementDetector& element)
    : tracker_(tracker),
      element_(element)
{
}

/* ================= 对外接口 ================= */
// TODO： 对外接口 process
void ImgCenterLine::process()
{
    adjust_center_line();

    uint8 prospect = get_prospect();   // 自动计算前瞻

    points_test_X=tracker_.center_line_[prospect];
    points_test_Y=prospect;
    calculate_error(prospect);
}

float ImgCenterLine::error() const
{
    return Det_True;
}

/* ================= 前瞻计算 ================= */
// TODO： 前瞻计算 get_prospect
uint8 ImgCenterLine::get_prospect()
{
    // uint8 prospect = 50;// 默认为50

    // uint8 prospect = tracker_.border_begin + 15 ;// 默认为50

    uint8 prospect = 45;// 默认为50

    //printf("border_begin = %d \n",tracker_.border_begin);

    auto& line_detect_center = tracker_.line_detect_center;

    /* ===== 直道 ===== */
    if (element_.Straight_Flag==1)
        prospect = 45; //直到速度快，不要提前转弯了

    // /* ===== 十字 ===== */
    if (element_.Cross_Flag==1 )
    {
        // printf("Cross_Flag =  1
        // prospect = tracker_.r_border_begin + 3;
         prospect = 45;
    }
    else if ( element_.Cross_Flag==2 )
    {
        prospect = 65;

        // prospect = 35;
        // if (element_.cross_fill_right) {          // 右十字
        //     for (int i = tracker_.l_border_begin;
        //          tracker_.l_border_[i] < HISTOGRAM_SEARCH_LEFT + 12 && i < 80;
        //          ++i) { prospect = i; }
        // } else {                                   // 左十字
        //     for (int i = tracker_.r_border_begin;
        //          tracker_.r_border_[i] > HISTOGRAM_SEARCH_RIGHT - 12 && i < 80;
        //          ++i) { prospect = i; }
        // }
    }
    else if (element_.Cross_Flag==3 || element_.Cross_Flag==4)
    {
        // if (element_.cross_fill_right) {          // 右十字
        //     for (int i = tracker_.l_border_begin;
        //          tracker_.l_border_[i] < HISTOGRAM_SEARCH_LEFT + 17 && i < 80;
        //          ++i) { prospect = i; }
        // } else {                                   // 左十字
        //     for (int i = tracker_.r_border_begin;
        //          tracker_.r_border_[i] > HISTOGRAM_SEARCH_RIGHT - 17 && i < 80;
        //          ++i) { prospect = i; }
        // }
        //
        // if (prospect > 78)
        // {
        //     printf("prospect error");
        //     if (element_.cross_fill_right) {          // 右十字
        //         for (int i = tracker_.l_border_begin;
        //              tracker_.l_border_[i] < HISTOGRAM_SEARCH_LEFT + 3 && i < 87;
        //              ++i) { prospect = i; } // 用
        //     } else {                                   // 左十字
        //         for (int i = tracker_.r_border_begin;
        //              tracker_.r_border_[i] > HISTOGRAM_SEARCH_RIGHT - 3 && i < 87;
        //              ++i) { prospect = i; }
        //     }// 不能给太远（小），不然会转弯
        //     printf("prospect =%d \n",prospect);
        // }
        // if (prospect > 95)
        // {
        //     prospect = 76; // 保底
        // }
        prospect = 65;

    }

    else if (element_.Cross_Flag==5)
    {
        prospect = 71;
        // prospect = tracker_.l_border_begin;
    }

    // region ==== 之前十字的前瞻

    // /* ===== 十字 ===== */
    // if (element_.Cross_Flag==1 )
    // {
    //     if (line_detect_center < 30)
    //     {
    //         for (int i = 70; i < RESULT_ROWS; i++)
    //         {
    //             if (tracker_.center_line_[i] > HISTOGRAM_SEARCH_LEFT+1
    //                 && tracker_.center_line_[i] < HISTOGRAM_SEARCH_RIGHT+1
    //                 && i < 85)
    //             {
    //                 prospect = i;
    //                 break;
    //             }
    //             if (i > 85)
    //             {
    //                 prospect = 50;
    //             }
    //         }
    //     }
    // }
    //
    // // endregion

    /* ===== 右圆环 ===== */
    if (element_.RightCirque_Flag==4)
    {
        prospect = 55; // 默认67
        for (int i = tracker_.r_border_begin;
             tracker_.r_border_[i] < HISTOGRAM_SEARCH_RIGHT + 10
             && i < 70;  // 用 i，不是 l_border_begin
             ++i)
        {
            prospect = i;
        }// 不能给太远（小），不然会转弯
    }
        // prospect = 55; // 默认67
    else if (element_.RightCirque_Flag==5)
    {
        prospect = 65; // 默认65
        // for (int i = tracker_.l_border_begin;
        //      tracker_.l_border_[i] < HISTOGRAM_SEARCH_LEFT + 17
        //      && i < 72;  // 用 i，不是 l_border_begin
        //      ++i)
        // {
        //     prospect = i;
        // }// 不能给太远（小），不然会转弯
    }
    else if (element_.RightCirque_Flag==6)
    {
         prospect = 78; // 默认70
        // for (int i = tracker_.r_border_begin + 1;
        //      tracker_.r_border_[i] < HISTOGRAM_SEARCH_RIGHT + 14
        //      && i < 74;  // 用 i，不是 l_border_begin
        //      ++i)
        // {
        //     prospect = i;
        // }// 不能给太远（小），不然会转弯
    }

    /* ===== 左圆环 ===== */
    if (element_.LeftCirque_Flag==4)
        // prospect = 55; // 默认65
    {
        prospect = 55; // 默认67
        for (int i = tracker_.l_border_begin + 7;
             tracker_.l_border_[i] > HISTOGRAM_SEARCH_LEFT -10
             && i < 70;  // 用 i，不是 l_border_begin
             ++i)
        {
            prospect = i;
        }// 不能给太远（小），不然会转弯
    }
    else if (element_.LeftCirque_Flag==5)
        // prospect = 46; // 默认60
    {
        prospect = 65; // 默认67
        // for (int i = tracker_.r_border_begin;
        //      tracker_.r_border_[i] > HISTOGRAM_SEARCH_RIGHT - 17
        //      && i < 72;  // 用 i，不是 l_border_begin
        //      ++i)
        // {
        //     prospect = i;
        // }// 不能给太远（小），不然会转弯
    }
    else if (element_.LeftCirque_Flag==6)
        // prospect = 74; // 默认70
    {
        prospect = 78; // 默认67
        // for (int i = tracker_.l_border_begin + 1;
        //      tracker_.l_border_[i] > HISTOGRAM_SEARCH_LEFT - 20
        //      && i < 70;  // 用 i，不是 l_border_begin
        //      ++i)
        // {
        //     prospect = i;
        // }// 不能给太远（小），不然会转弯
    }
    /* ===== 坡道 ===== */
    if (element_.Slope_flag==1)
        prospect = 85;

    /* ===== 安全限制 ===== */
    if (prospect < line_detect_center)
        prospect = line_detect_center;

    if (prospect >= RESULT_ROWS - weight_size)
        prospect = RESULT_ROWS - weight_size - 1;

    return prospect;
}

/* ================= 中线策略 ================= */
// TODO： 中线策略 adjust_center_line
void ImgCenterLine::adjust_center_line()
{
    // 获取相关数据的引用
    auto& center_line          = tracker_.center_line_;
    // auto& center_last          = tracker_.center_line_last_; 未用上
    auto& l_border             = tracker_.l_border_;
    auto& r_border             = tracker_.r_border_;
    auto& line_detect_center   = tracker_.line_detect_center;

    // if (strategy_state == 1) {
    //     //printf("==== strategy_state ====");
    //     if (turn_dir > 0) {
    //         for (int i = RESULT_ROWS - 1; i > line_detect_center; i--) {
    //             center_line[i] = l_border[i];
    //         }
    //         center_line[RESULT_ROWS] = 0;
    //     } else {
    //         for (int i = RESULT_ROWS- 1; i > line_detect_center; i--) {
    //             center_line[i] = r_border[i];
    //
    //         }
    //         center_line[RESULT_ROWS] = 1;
    //     }
    //     return;
    // }

    /**
     * @brief 从指定边线填充中线的Lambda函数
     * @param border    使用的边线数组（左或右）
     * @param side_flag 边线标志（0=左边线，1=右边线）
     */
    auto fill_from_border = [&](uint8* border, uint8 side_flag)
    {
        tracker_.use_left_border_only_ = false;
        tracker_.use_right_border_only_ = false;
        // 从下往上填充中线
        for (int i = RESULT_ROWS - 2; i >= line_detect_center; i--) // -2防止循环中数组越界
        {
            center_line[i] = border[i];                     // 直接用边线作为中线

            if ((element_.Cross_Flag ==1
                // || element_.Cross_Flag ==5
                )
                &&(center_line[i] ==  INVALID_BORDER || center_line[i] == 0))

            {   // 十字状态1特殊处理 无效点（丢线）用对应基准值填充，不再向上传播
                {
                    center_line[i] = (side_flag == 0)
                                     ? HISTOGRAM_SEARCH_LEFT    // 68
                                     : HISTOGRAM_SEARCH_RIGHT;  // 97
                }
            }

            // 处理丢失点（无效点）
            else if (center_line[i] == INVALID_BORDER)
            {
                center_line[i] = center_line[i + 1];        // 用下一行的值填充
            }
        }

        // 记录使用的是哪条边线（用于后续处理）
        // 0=左边线, 1=右边线
        if (side_flag == 0)
        {
            tracker_.use_left_border_only_ =true;
        }
        else if (side_flag == 1)
        {
            tracker_.use_right_border_only_ =true;
        }
    };

    if (strategy_state == 1) {
        //printf("==== strategy_state ====");
        if (turn_dir > 0)
        {
            fill_from_border(l_border, 0);
        }
        else
        {
            fill_from_border(r_border, 1);

        }
        return;
    }

    // printf("cheshi \n");
    /* ===== 根据元素类型选择中线策略 ===== */
    // ===== 右圆环 =====
    if (element_.RightCirque_Flag == 4 ||
        element_.RightCirque_Flag == 6)
    {
        fill_from_border(r_border, 1);
    }
    else if (element_.RightCirque_Flag == 2 ||
             element_.RightCirque_Flag == 3 ||
             element_.RightCirque_Flag == 5 ||
             element_.RightCirque_Flag == 7)
    {
        fill_from_border(l_border, 0);
    }

    // ===== 左圆环 =====
    else if (element_.LeftCirque_Flag == 4 ||
             element_.LeftCirque_Flag == 6  )
    {
        fill_from_border(l_border, 0);
    }
    else if (element_.LeftCirque_Flag == 2 ||
             element_.LeftCirque_Flag == 3 ||
             element_.LeftCirque_Flag == 5 ||
             element_.LeftCirque_Flag == 7)
    {
        fill_from_border(r_border, 1);
    }

    // ===== 十字 =====
    else if (element_.Cross_Flag == 1 )
    {
        if (l_ok)
        {
            fill_from_border(l_border, 0);   // 右十字：寻左线
        }
        else
        {
            fill_from_border(r_border, 1);   // 默认左十字：寻右线
        }
    }
    else if ( element_.Cross_Flag == 2 ||
              element_.Cross_Flag == 3 || element_.Cross_Flag == 4 || element_.Cross_Flag == 5)  // 十字状态一可以不用特殊单边寻线
    {
        if (element_.cross_fill_right) {
            fill_from_border(l_border, 0);   // 右十字：寻左线
        }
        else {
            fill_from_border(r_border, 1);   // 默认左十字：寻右线
        }
    }

    else if (element_.R_Obstacle_flag)
    {
        fill_from_border(r_border, 1);
    }
    else if (element_.L_Obstacle_flag)
    {
        fill_from_border(l_border, 0);
    }
    // else if (element_.Cross_Flag == 5)
    // {
    //     if (element_.cross_fill_right) {
    //         fill_from_border(r_border, 1);   // 左十字出十字：右边界已恢复，切回右线
    //     }
    //     else {
    //         fill_from_border(l_border, 0);   // 右十字出十字：切回左线
    //     }
    // }

    // ===== 普通赛道 =====
    else
    {
        if (tracker_.l_border_lostnum <= tracker_.r_border_lostnum)
        {
            fill_from_border(l_border, 0);
        }
        else
        {
            fill_from_border(r_border, 1);
        }
    }

}

/* ================= 误差计算 ================= */
// TODO： 误差计算 calculate_error
void ImgCenterLine::calculate_error(uint8 prospect)
{
    auto& center_line = tracker_.center_line_;
    auto& center_last = tracker_.center_line_last_;

    float sum = 0.0f;
    float weight_sum = 0.0f;

    float mid_line = 0;


    /* 确定基准中线位置 */
    if (tracker_.use_left_border_only_) {
        mid_line = HISTOGRAM_SEARCH_LEFT;

        if (strategy_state == 1)
            mid_line +=35;
    }
    else if (tracker_.use_right_border_only_) {
        mid_line = HISTOGRAM_SEARCH_RIGHT;
        if (strategy_state == 1)
            mid_line -= 35;
    }

    // /* ========== 打印右边边界数据（调试用） ========== */
    // static int print_cnt = 0;
    // if (++print_cnt >= 30) {          // 每 30 帧打印一次，防止串口刷屏
    //     print_cnt = 0;
    //     printf("--- Right Border [prospect:%d, prospect+weight:%d] ---\n",
    //            prospect, prospect + weight_size);
    //     for (uint8 j = prospect; j < prospect + weight_size && j < RESULT_ROWS; j++) {
    //         printf("  y=%3d: right=%3d, center=%3d\n",
    //                j, tracker_.r_border_[j], center_line[j]);
    //     }
    //     printf("------------------------------------------------\n");
    // }
    /* ===
    // /* 只有 mid_line 变化才打印 */
    // if (mid_line != last_mid_line)
    // {
    //     if (mid_line == HISTOGRAM_SEARCH_LEFT)
    //         printf("mid_line = HISTOGRAM_SEARCH_LEFT\n");
    //     else if (mid_line == HISTOGRAM_SEARCH_RIGHT)
    //         printf("mid_line = HISTOGRAM_SEARCH_RIGHT\n");
    //
    //     last_mid_line = mid_line;
    // }

    for (uint8 j = prospect; j < prospect + weight_size; j++)
    {
        uint8 idx = j - prospect;

        if (j >= RESULT_ROWS)
        {
            center_line[j] = center_line[j - 1];
        }

        center_last[j] = center_line[j];

        sum += center_line[j] * Weight2[idx];
    }

    for(uint8 x=0;x<weight_size;x++)
    {
        weight_sum+=Weight2[x];
    }

    if (weight_sum > 0.0f)
        Det_True = sum / weight_sum - mid_line;
    else
        Det_True = 0.0f;

    /* 一阶低通滤波 */
    Det_True = -(Det_True * 0.8f + last_Det_True * 0.2f);
    //printf("error=%f\n",Det_True);

    if (element_.Zebra_crossing_flag == 1 )
        Det_True = 0.0f;

    if (element_.Slope_flag)
    {
        constexpr int SLOPE_START = 80;      // 起始行
        constexpr int SLOPE_END   = 92;    // 结束行（不含），即取 80~91
        constexpr int SLOPE_ROWS  = SLOPE_END - SLOPE_START;  // 12 行

        float slope_sum  = 0.0f;   // 加权中线累加
        float slope_wsum = 0.0f;   // 权重累加
        float mid_ref = (HISTOGRAM_SEARCH_LEFT + HISTOGRAM_SEARCH_RIGHT) / 2.0f;

        for (int i = 0; i < SLOPE_ROWS; ++i)
        {
            int y = SLOPE_START + i;
            uint8 l = tracker_.l_border_[y];
            uint8 r = tracker_.r_border_[y];

            /* 任一边丢线则跳过该行，不参与平均 */
            if (l == INVALID_BORDER || r == INVALID_BORDER)
                continue;

            /* 左右边线平均得到该行实际中线 */
            float center_x = (l + r) / 2.0f;

            /* 复用 Weight2 的前 12 个权重；若 weight_size < 12 则后面退化为 1.0 */
            float w = (i < weight_size) ? Weight2[i] : 1.0f;

            slope_sum  += center_x * w;
            slope_wsum += w;
        }

        /* 加权平均后减参考中线，再走一阶低通（与普通逻辑保持同阶滤波） */
        if (slope_wsum > 0.0f)
        {
            float raw = slope_sum / slope_wsum - mid_ref;
            Det_True = -(raw * 0.9f + last_Det_True * 0.1f);
        }
        // 若 12 行全丢线，保持当前 Det_True 不变（或你也可置 0）
    }
    //
    // if (element_.Cross_Flag == 4 && prospect == 79)
    // {
    //     if (element_.cross_fill_right) {Det_True -= 4;}
    //     else {Det_True += 4;}
    //     printf("Det_True = %f \n", Det_True);
    // }
    // printf("Det_True = %f \n",Det_True);
    last_Det_True = Det_True;
}