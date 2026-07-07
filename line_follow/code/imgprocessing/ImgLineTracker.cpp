//
// Created by ros on 2026/1/29.
////
// Created by ros on 2026/1/28.
//

#include "ImgLineTracker.h"
#include "ImgConfig.h"
#include <cstring>

/* ================= 构造函数 ================= */

ImgLineTracker::ImgLineTracker()
{
}

/* ================= 对外主流程 ================= */
// TODO: 对外主流程 process
void ImgLineTracker::process(const cv::Mat& ipm_image)
{
    /* 清理统计数据 */
    data_stastics_l = 0;
    data_stastics_r = 0;
    line_detect_center = USE_H-1;
    // /* 1. 获取起始点（通常从底部开始） */
    if (get_start_point(ipm_image))
    /* 2. 八邻域搜索左右边线用于元素检测 */
    {
        search_l_r(USE_NUM,
                   ipm_image,
                   start_point_l_[0],
                   start_point_l_[1],
                   start_point_r_[0],
                   start_point_r_[1],
                   &hightest_);
    };

    //最长白列，获取左右边线，中心行
    vgetborder_maxwhitecols(ipm_image);

    CountLostLines();

}

/* ================= 起点搜索 ================= */
// TODO: 起点搜索 get_start_point
uint8_t ImgLineTracker::get_start_point(const cv::Mat& bin_image) {

    start_point_l_[0] = 0;//x
    start_point_l_[1] = 0;//y

    start_point_r_[0] = 0;//x
    start_point_r_[1] = 0;//y
    bool l_found = false, r_found = false;

    const int mid  = RESULT_COLS / 2;// 列
    // 从中间向左寻找
    for (int i = mid; i > BORDER_MIN; --i)
    {
        if (bin_image.at<uint8_t>(START_H, i) == COLOR_WHITE &&
            bin_image.at<uint8_t>(START_H, i - 1) != COLOR_WHITE)
            /// 跳变点
        {
            start_point_l_[0] = i;//x
            start_point_l_[1] = START_H;// y
            l_found = true;
            break;
        }
    }

    // 从中间向右寻找
    for (int i = mid; i < RESULT_COLS-1; ++i)
    {
        if (bin_image.at<uint8_t>(START_H, i) == COLOR_WHITE &&
            bin_image.at<uint8_t>(START_H,i + 1) != COLOR_WHITE)
        {
            start_point_r_[0] = i;//x
            start_point_r_[1] = START_H;
            r_found = true;
            break;
        }
    }

    return l_found && r_found;  /// 左右八领域起点均找到才算成功
}

/* ================= 八邻域搜索 ================= */
// TODO：八邻域搜索 search_l_r
void ImgLineTracker::search_l_r(
    uint16_t break_flag,    // 最大搜索步数（防死循环）
    const cv::Mat& image,   // 二值图像（255 = 白）
    uint8_t l_start_x,      // 左边线起点 x
    uint8_t l_start_y,      // 左边线起点 y
    uint8_t r_start_x,      // 右边线起点 x
    uint8_t r_start_y,      // 右边线起点 y
    uint8_t* hightest_)    // 输出：左右相遇时的最高 y        // 新增：截至行（最小 y），对应 offline
{
    uint8 i = 0, j = 0;

    // region =======左边变量=======

    uint8 search_filds_l[8][2] = { {  0 } };
    uint8 index_l = 0;
    uint8 temp_l[8][2] = { {  0 } };
    uint8 center_point_l[2] = { 0 };
    uint16 l_data_statics;//统计左边
    //定义八个邻域
    static int8 seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,  0},{1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是顺时针
    // endregion

    // region =======右边变量=======，有注释

    uint8 search_filds_r[8][2] = { {  0 } }; //八邻域 8 个候选坐标
    uint8 index_r = 0;                  // temp_l 里已经存了多少个“合法候选点”
    uint8 temp_r[8][2] = { {  0 } }; //保存所有满足“黑→白跳变”的候选点
    uint8 center_point_r[2] = { 0 };    //中心坐标点
    uint16 r_data_statics;              //右边线已经爬到的点数
    //定义八个邻域
    static int8 seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是逆时针
    // endregion

    l_data_statics = data_stastics_l;//统计找到了多少个点，方便后续把点全部画出来
    r_data_statics = data_stastics_r;//统计找到了多少个点，方便后续把点全部画出来

    //第一次更新坐标点  将找到的起点值传进来
    center_point_l[0] = l_start_x;//x
    center_point_l[1] = l_start_y;//y
    center_point_r[0] = r_start_x;//x
    center_point_r[1] = r_start_y;//y

    //开启邻域循环
    while (break_flag--)
    {
        /* ========== 新增：爬到截至行，强制退出 ========== */
        if (center_point_l[1] <= offline || center_point_r[1] <= offline)
        {
            *hightest_ = std::min(center_point_l[1], center_point_r[1]);
            break;
        }

        // region =======左边生成八邻域，记录中心点=======

        for (i = 0; i < 8; i++)//传递8F坐标
        {
            search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];//x
            search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];//y
        }
        //中心坐标点填充到已经找到的点内
        points_l[l_data_statics][0] = center_point_l[0];//x
        points_l[l_data_statics][1] = center_point_l[1];//y
        l_data_statics++;//左索引加一
        // endregion

        // region =======右边生成八邻域，记录中心点=======，有注释

        for (i = 0; i < 8; i++)//传递8F坐标
        {   // 中心点加种子8个坐标，得到8领域
            search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
            search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
        }
        //中心坐标点填充到边线数组中
        points_r[r_data_statics][0] = center_point_r[0];//x
        points_r[r_data_statics][1] = center_point_r[1];//y

        // endregion

        // region =======左边开始爬线=======

        index_l = 0;//先清零，后使用
        memset(temp_l, 0, sizeof(temp_l));
        //左边判断
        for (i = 0; i < 8; i++)
        {
            if (image.at<uint8_t>(search_filds_l[i][1], search_filds_l[i][0]) != COLOR_WHITE
                && image.at<uint8_t>(search_filds_l[(i + 1) & 7][1],
                                     search_filds_l[(i + 1) & 7][0]) == COLOR_WHITE)
            {   /// 当前点不是白色，且下一邻域点是白色
                temp_l[index_l][0] = search_filds_l[(i)][0];
                temp_l[index_l][1] = search_filds_l[(i)][1];
                index_l++;
                dir_l[l_data_statics - 1] = (i);//记录生长方向
            }
            if (index_l)
            {
                //更新坐标点
                center_point_l[0] = temp_l[0][0];//x
                center_point_l[1] = temp_l[0][1];//y

                for (j = 0; j < index_l; j++)
                {// 如果有多个候选点，选择 y 最小的（最靠上）
                    if (center_point_l[1] > temp_l[j][1])
                    {
                        center_point_l[0] = temp_l[j][0];//x
                        center_point_l[1] = temp_l[j][1];//y
                    }
                }
            }
        }
        // printf("temp_l[0] = %d, temp_l[1] = %d\n", temp_l[0][0], temp_l[0][1]);
        // printf("points_l[l_data_statics - 1]=%d %d,%d,%d,%d,%d\n",points_l[l_data_statics - 1][0],points_l[l_data_statics - 1][1],points_l[l_data_statics - 2][0],points_l[l_data_statics - 2][1],points_l[l_data_statics - 3][0],points_l[l_data_statics - 3][1]);
        // endregion

        // region =======退出条件判断=======
        if ((points_r[r_data_statics][0] == points_r[r_data_statics - 1][0] &&
             points_r[r_data_statics][0] == points_r[r_data_statics - 2][0] &&
             points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] &&
             points_r[r_data_statics][1] == points_r[r_data_statics - 2][1])
            ||
            (points_l[l_data_statics - 1][0] == points_l[l_data_statics - 2][0] &&
             points_l[l_data_statics - 1][0] == points_l[l_data_statics - 3][0] &&
             points_l[l_data_statics - 1][1] == points_l[l_data_statics - 2][1] &&
             points_l[l_data_statics - 1][1] == points_l[l_data_statics - 3][1]))
        {
            //printf("三次进入同一个点，退出\n");
            break;
        }
        // 左右相遇，退出
        if (abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2
            && abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1]) < 2)
        {
            //printf("\n左右相遇退出\n");
            *hightest_ =//hightest 表示：左右边线在第几行“相遇 / 终止”，代码后面有用
                (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;
            //printf("\n在y=%d处退出\n",*hightest);
            break;
        }

        // 左边比右边高，等待
        if (points_r[r_data_statics][1] < points_l[l_data_statics - 1][1])
        {
            //printf("\n如果左边比右边高了，左边等待右边\n");
            continue;//如果左边比右边高了，左边等待右边
        }

        //如果左边比右边高了，左边等待右边，左边向下生长了
        if (dir_l[l_data_statics - 1] == 7
            && (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1]))//左边比右边高且已经向下生长了
        {
            //printf("\n左边开始向下了，等待右边，等待中... \n");
            center_point_l[0] = (uint8)points_l[l_data_statics - 1][0];//x
            center_point_l[1] = (uint8)points_l[l_data_statics - 1][1];//y
            l_data_statics--;
        }

        // endregion

        // region =======右边开始爬线=======,有注释

        r_data_statics++;//右索引加一
        index_r = 0;//先清零，后使用 temp_l 里已经存了多少个“合法候选点”
        memset(temp_r, 0, sizeof(temp_r));// 将temp数组置零

        //右边判断
        for (i = 0; i < 8; i++)
        {   // 这里的i指的第8领域的第i个点
            if (image.at<uint8_t>(search_filds_r[i][1], search_filds_r[i][0]) != COLOR_WHITE
                && image.at<uint8_t>(search_filds_r[(i + 1) & 7][1],
                                     search_filds_r[(i + 1) & 7][0]) == COLOR_WHITE)
            {   /// 当前点不是白色，且下一邻域点是白色，检测“黑 → 白”的边界跳变
                temp_r[index_r][0] = search_filds_r[(i)][0];
                temp_r[index_r][1] = search_filds_r[(i)][1];
                index_r++;//索引加一，存的“合法候选点”个数
                dir_r[r_data_statics - 1] = (i);//左边边线第 r_data_statics 层边线的生长方向为i

                //printf("dir[%d]:%d\n", r_data_statics, dir_r[r_data_statics]);
            }
            if (index_r)
            {
                //更新坐标点
                center_point_r[0] = temp_r[0][0];//x
                center_point_r[1] = temp_r[0][1];//y

                for (j = 0; j < index_r; j++)
                {   //如果有多个候选点，选择 y 最小的（最靠上）
                    if (temp_r[j][1] > center_point_r[1])
                    {
                        center_point_r[0] = temp_r[j][0];//x
                        center_point_r[1] = temp_r[j][1];//y
                    }
                    // printf("temp_r[0] = %d, temp_r[1] = %d\n", temp_r[0][0], temp_r[0][1]);
                }
            }
        }
        // endregion

    }

    //取出循环次数
    data_stastics_l = l_data_statics;
    data_stastics_r = r_data_statics;

}

/* ================= 左右边线生成 ================= */

// TODO: 八领域左边线生成 get_left
void ImgLineTracker::get_left(uint16 total_L)
{
    uint8 i = 0;
    uint16 j = 0;
    uint8 h = 0;

    //初始化
    for (i = 0; i < RESULT_ROWS; i++)
    {
        l_border_[i] = INVALID_BORDER;
    }
    h = START_H;
    //左边

    for (j = 0; j < total_L; j++)
    {
        if (points_l[j][1] == h)
        {
            l_border_[h] = points_l[j][0] + 1;
        }
        else continue; //每行只取一个点，没到下一行就不记录

        h--;

        if (h == 0)
        {
            break;//到最后一行退出
        }
    }

}

// TODO：八领域的右边线生成 get_right
void ImgLineTracker::get_right(uint16_t total_R)
{
    uint8 i = 0;
    uint16 j = 0;
    uint8 h = 0;
    for (i = 0; i < RESULT_ROWS; i++)
    {
        r_border_[i] = INVALID_BORDER;//右边线初始化放到最右边，左边线放到最左边，这样八邻域闭合区域外的中线就会在中间，不会干扰得到的数据
    }
    h = START_H;
    //右边
    for (j = 0; j < total_R; j++)
    {
        if (points_r[j][1] == h)
        {
            r_border_[h] = points_r[j][0] - 1;
        }
        else continue;//每行只取一个点，没到下一行就不记录

        h--;

        if (h == 0)break;//到最后一行退出
    }

}

// TODO: 八领域左边线提取 left_get
void ImgLineTracker::left_get(uint16_t total_L)
{
    uint8 i = 0;
    uint16 j = 0;
    uint8 h = 0;

    //初始化
    for (i = 0; i < RESULT_ROWS; i++)
    {
        border_l_[i] = INVALID_BORDER;
    }
    h = START_H;
    //左边

    for (j = 0; j < total_L; j++)
    {
        if (points_l[j][1] == h)
        {
            border_l_[h] = points_l[j][0] + 1;
        }
        else continue; //每行只取一个点，没到下一行就不记录

        h--;

        if (h == 0)
        {
            break;//到最后一行退出
        }
    }

}

// TODO：八领域的右边线提取 right_get
void ImgLineTracker::right_get(uint16_t total_R)
{
    uint8 i = 0;
    uint16 j = 0;
    uint8 h = 0;
    for (i = 0; i < RESULT_ROWS; i++)
    {
        border_r_[i] = INVALID_BORDER;//右边线初始化放到最右边，左边线放到最左边，这样八邻域闭合区域外的中线就会在中间，不会干扰得到的数据
    }
    h = START_H;
    //右边
    for (j = 0; j < total_R; j++)
    {
        if (points_r[j][1] == h)
        {
            border_r_[h] = points_r[j][0] - 1;
        }
        else continue;//每行只取一个点，没到下一行就不记录

        h--;

        if (h == 0)break;//到最后一行退出
    }

}

/* ================= 最长白列搜索  ================= */
// TODO： 最长白列 vgetborder_maxwhitecols
void ImgLineTracker::vgetborder_maxwhitecols(const cv::Mat& image)
{
    // region =======获取最长白列的长度=======
    // 每次执行前清零边界数组！
    for (int i = 0; i < RESULT_ROWS; i++) {
        l_border_[i] = INVALID_BORDER;
        r_border_[i] = INVALID_BORDER;
    }
    int White_Column[USED_COL] = {0}; //每列白列长度
    //获取白色直方图
    for (int lie =2; lie<RESULT_COLS-3; lie++)// 大循环 从左往右
    {
        for (int hang = RESULT_ROWS - 1; hang > offline; hang--)// 小循环 从下往上
        {
            if (image.at<uint8_t>(hang, lie)!=COLOR_WHITE)
            {
                // std::cout << "第 " << hang << " 行遇到黑色，中断扫描" << std::endl;
                break;
            }
            White_Column[lie]++;// 获取列的白色像素点的数量
        }
    }
    // endregion

    // region =======获取最长白列起始行坐标=======

    //获取最长白列位置，0是长度一列白色像素点的个数 ，1是位置最长列
    unsigned char ucl_hightestcol[2]={0}, ucr_hightestcol[2]={0};
    // 从左到右扫描
    for (int i=HISTOGRAM_SEARCH_LEFT;i<HISTOGRAM_SEARCH_RIGHT;i++)
        // 选择60 99的原因 在直道上左边界是59 右边界是103 为了节省算力 选用60-99 建议不修改范围
    {
        if (ucr_hightestcol[0]<White_Column[i]) {
            ucr_hightestcol[0]=White_Column[i];//0是长度 y
            ucr_hightestcol[1]=i;//1是位置
        }
    }
    // 从右到左扫描
    for (int i=HISTOGRAM_SEARCH_RIGHT;i>HISTOGRAM_SEARCH_LEFT;i--)//宽度
    {
        if (ucl_hightestcol[0]<White_Column[i]) {
            ucl_hightestcol[0]=White_Column[i];//0是长度
            ucl_hightestcol[1]=i;//1是位置
        }
    }
    ucr_hightestcol[0]=RESULT_ROWS-1-ucr_hightestcol[0];//由于y轴是向下的 实际的y坐标需要用图像的高度减去白色像素点的数目
    ucl_hightestcol[0]=RESULT_ROWS-1-ucl_hightestcol[0];
    line_detect_left=ucl_hightestcol[0];  //最长白列的有效点的起始坐标
    line_detect_right=ucr_hightestcol[0];
    line_detect_center = std::max(ucl_hightestcol[0], ucr_hightestcol[0]); // 获取中心行的起始行数
    // endregion

    // region =======获取左右边界数组=======
    // r_border_lostnum=0;
    // l_border_lostnum=0;
    // 中心行向下扫描，寻找左右边界
    for (int i=RESULT_ROWS-1;i>=line_detect_center;i--)
    {
        // 右最长白列向右边扫描，j是x坐标
        for (int j=ucr_hightestcol[1];j<=RESULT_COLS-1-2;j++) //数组减-1 边框-2
        {
            if (image.at<uint8_t>(i, j)==COLOR_WHITE &&
                image.at<uint8_t>(i, j+1)<= COLOR_GRAY &&
                image.at<uint8_t>(i, j+2)<= COLOR_GRAY &&
                image.at<uint8_t>(i, j+15)!= COLOR_WHITE
                //&& image.at<uint8_t>(i, j+15)<= COLOR_GRAY // 防止识别到图片
                ) {
                r_border_[i]=j;// 第i行中x坐标为j的点是右边界点
                // printf("r_border[i]=%d\n",r_border[i]);
                break;
            }
            // j一直找到了115列依旧没有边界
            if (j>=LOST_BORDER_RIGHT)
            {
                r_border_[i]=INVALID_BORDER;
                // r_border_lostnum++; // 后面统一判断寻线
                break;
            }
            //printf("r_border_lostnum=%d\n",r_border_lostnum);
        }

        /// 左最长白列向左边扫描
        for (int j=ucl_hightestcol[1];j>=2;j--)
        {
            if (image.at<uint8_t>(i, j  ) == COLOR_WHITE &&
                image.at<uint8_t>(i, j-1) <= COLOR_GRAY &&
                image.at<uint8_t>(i, j-2) <= COLOR_GRAY&&
                image.at<uint8_t>(i, j-15) != COLOR_WHITE)
            {
                l_border_[i]=j;
                // not
                 //printf("l_border[i]=%d\n",l_border_[i]);
                break;
            }
            if (j<LOST_BORDER_LEFT)
            {
                l_border_[i]=INVALID_BORDER;
                // l_border_lostnum++;//lost // 后面统一判断寻线
                //printf("l_border_lostnum=%d\n",l_border_lostnum);
                break;
            }
        }
        // endregion

    }
}

void ImgLineTracker::CountLostLines()
{
    l_border_lostnum = 0;
    r_border_lostnum = 0;
    lost_count = 0;
    valid_count = 0;
    r_border_begin = 255;
    l_border_begin = 255;

    for (int i =line_detect_center ;i < RESULT_ROWS - 1;++i)
    {
        if (r_border_begin == 255)
        {
            if (r_border_[i] != INVALID_BORDER)
                r_border_begin = i;
        }

        if (l_border_begin == 255)
        {
            if (l_border_[i] != INVALID_BORDER)
                l_border_begin = i;
        }

        border_begin = std::min(r_border_begin, l_border_begin); // 获取边线起始点(从上往下)

        if (l_border_[i] == INVALID_BORDER)
        {
            l_border_lostnum++;
        }
        if (r_border_[i] == INVALID_BORDER)
        {
            r_border_lostnum++;
        }

        if (l_border_[i] == INVALID_BORDER &&
            r_border_[i] == INVALID_BORDER)
        {
            lost_count++;
        }
        if (l_border_[i] != INVALID_BORDER &&
            r_border_[i] != INVALID_BORDER)
        {
            valid_count++;
        }

    }
    /* ========== 边线连续性检查（仅看数值跳变，丢行直接跳过） ========== */
    constexpr uint8_t MAX_EDGE_JUMP = 10;   // 相邻有效行边线列坐标最大允许差值

    // ----- 左边界 -----
    l_border_continuous = false;
    if (l_border_begin != 255)              // 至少有一个有效点才算连续
    {
        l_border_continuous = true;
        uint8_t last_valid = l_border_[l_border_begin];  // 基准：起始行有效值

        for (int i = l_border_begin + 1; i < RESULT_ROWS; ++i)
        {
            if (l_border_[i] == INVALID_BORDER)
                continue;                   // 丢线行直接继承前面，不参与判断

            // 只关心：当前有效值 vs 上一个有效值 的列跳变
            if (abs((int)l_border_[i] - (int)last_valid) > MAX_EDGE_JUMP)
            {
                l_border_continuous = false;
                break;
            }
            last_valid = l_border_[i];        // 更新基准
        }
    }

    // ----- 右边界 -----
    r_border_continuous = false;
    if (r_border_begin != 255)
    {
        r_border_continuous = true;
        uint8_t last_valid = r_border_[r_border_begin];

        for (int i = r_border_begin + 1; i < RESULT_ROWS; ++i)
        {
            if (r_border_[i] == INVALID_BORDER)
                continue;

            if (abs((int)r_border_[i] - (int)last_valid) > MAX_EDGE_JUMP)
            {
                r_border_continuous = false;
                break;
            }
            last_valid = r_border_[i];
        }
    }

    /* ========== 记录边线起点变化值 ========== */
    // 计算当前帧与上一帧的差值（带符号，可正可负）
    delta_r_border_begin = (int16_t)r_border_begin - (int16_t)last_r_border_begin;
    delta_l_border_begin = (int16_t)l_border_begin - (int16_t)last_l_border_begin;
    delta_border_begin   = (int16_t)border_begin   - (int16_t)last_border_begin;

    // 更新历史值，供下一帧使用
    last_r_border_begin = r_border_begin;
    last_l_border_begin = l_border_begin;
    last_border_begin   = border_begin;

#if LINE_TRACKER_DEBUG
    // 仅在当前帧有有效起点时打印，减少刷屏
    if (border_begin != 255)
    {
        printf("BorderDelta: L=%3d(%+4d) R=%3d(%+4d) B=%3d(%+4d)\n",
               l_border_begin, delta_l_border_begin,
               r_border_begin, delta_r_border_begin,
               border_begin,   delta_border_begin);
    }
#endif



    // std::cout << "tracker_.l_border_lostnum = " <<static_cast<int>(tracker_.l_border_lostnum)  << std::endl;
    // std::cout << "tracker_.r_border_lostnum= " << static_cast<int>(tracker_.r_border_lostnum) << std::endl;

}

bool ImgLineTracker::fit_and_correct_border(uint8_t* border)
{
    // 第一步：收集有效点（非255）且连续（与上一个有效点x差≤7）
    int valid_x[RESULT_ROWS], valid_y[RESULT_ROWS], valid_count = 0;
    int last_valid_x = -1;

    for (int i = 2; i < RESULT_ROWS - 2; i++)
    {
        uint8_t val = border[i];
        if (val == INVALID_BORDER)
            continue;

        if (last_valid_x >= 0 && std::abs((int)val - last_valid_x) > 7)
            continue;

        valid_x[valid_count] = val;
        valid_y[valid_count] = i;
        valid_count++;
        last_valid_x = val;
    }

    if (valid_count < 10)
        return false;          // ✅ 补线失败：有效点太少

    // 第二步：最小二乘拟合直线 x = a*y + b
    double sum_y = 0, sum_x = 0, sum_yy = 0, sum_xy = 0;
    for (int i = 0; i < valid_count; i++)
    {
        sum_y += valid_y[i];
        sum_x += valid_x[i];
        sum_yy += valid_y[i] * valid_y[i];
        sum_xy += valid_y[i] * valid_x[i];
    }

    double denom = valid_count * sum_yy - sum_y * sum_y;
    if (fabs(denom) < 1e-6)
        return false;          // ✅ 补线失败：除零保护

    double a = (valid_count * sum_xy - sum_y * sum_x) / denom;
    double b = (sum_x - a * sum_y) / valid_count;

    // 第三步：修正所有点（包括255的无效点）
    for (int i = 2; i < RESULT_ROWS - 2; i++)
    {
        int x_expected = (int)(a * i + b);
        x_expected = std::max(0, std::min(RESULT_COLS - 1, x_expected));

        if (border[i] == INVALID_BORDER ||
            abs(border[i] - x_expected) > 5)
        {
            border[i] = (uint8_t)x_expected;
        }
    }

    return true;               // ✅ 补线成功
}