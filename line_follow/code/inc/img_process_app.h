// #ifndef IMG_PROCESS_H
// #define IMG_PROCESS_H
//
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/opencv.hpp>
// #include <iostream>
// #include <vector>
// #include <stdio.h>
// #include <chrono>
// #include "zf_common_headfile.h"
// #include "kalman_filter.h"
// #include "img_math.h"
// #include "imu.h"
//
// // typedef unsigned char       uint8;
// // typedef unsigned short       uint16;
// // typedef  short         int16;
// // typedef char         int8;
// // #define PI 3.14159265358979323846
// #define ImageUsed   *PerImg_ip
// #define border_max		use_w-1 //边界宽度
// #define border_min		1 //边界宽度
// #define         USED_COL                160
// #define         USED_ROW                120
// #define image_h	480   //原始图像大小
// #define image_w	640
// #define compressimage_h	120  //压缩后的图像大小
// #define compressimage_w	160
// #define RESULT_ROW 100
// #define RESULT_COL 160//
// #define llost_border 45
// #define rlost_border 115
// #define use_h compressimage_h   //要处理的图像大小 image_h 或者compressimage_h
// #define use_w compressimage_w
// // #define max(a,b)	(((a) > (b)) ? (a) : (b))
// // #define min(a,b)	(((a) < (b)) ? (a) : (b))
//
// using std::min;
// using std::max;
// #define start_h RESULT_ROW - 1 //八领域开始的生长y
// //可使用的
// //#include <ros/ros.h>
//
// class img_process_object
// {
// public:
//
//
//     /*逆透视*/
//
//
//     cv::Mat inversePerspectiveTransform(const cv::Mat& src, const cv::Mat& inverseMatrix, int result_rows, int result_cols);
//
//     /*逆透视*/
//     //权重参数
//
//     /*元素*/
//     uint8 RightCirque_Flag = 0;
//     uint8 LeftCirque_Flag = 0;
//     uint8 Straight_Flag = 0;
//     uint8 Cross_adxl =0;
//     uint8 Cross_adxr =0;
//     uint8 Cross_Flag = 0;
//     uint8 Stop_Flag = 0;
//     bool Zebra_crossing_flag=false;
//     bool Slope_flag=false;
//     bool Cube_flag=false;
//     bool R_Obstacle_flag=false;
//     bool L_Obstacle_flag=false;
//     int bend_flag = 0;
//
//     /*元素*/
//     /*里程计部分*/
//     double total_pulse;//总里程计
//     double last0_pulse;//开启后记录的里程计
//     double last1_pulse;
//     double last2_pulse;
//     double last3_pulse;
//     double now0_pulse;//当前行驶的距离
//     double now1_pulse;
//     double now2_pulse;
//     double now3_pulse;
//     /*里程计部分*/
//
//
//
//     /*寻线*/
//     #define USE_num use_h*9
//     uint8 offline = 20;//截至行,截至行后的数据为无效数据
//     uint16 data_stastics_l = 0;
//     uint16 data_stastics_r = 0;
//     uint8 l_border[use_h];//左边线
//     uint8 r_border[use_h];//右边线
//     uint8 border_l[use_h];//左边线
//     uint8 border_r[use_h];//右边线
//     uint8 l_border_lostnum;
//     uint8 r_border_lostnum;
//     uint8 center_line[use_h] ={};//中线数组
//     uint8 center_line_last[use_h] ={};//中线数组
//
//     uint16 points_l[(uint16)USE_num][2] = { {  0 } };
//     uint16 points_r[(uint16)USE_num][2] = { {  0 } };
//     uint16 dir_r[(uint16)USE_num] = { 0 };
//     uint16 dir_l[(uint16)USE_num] = { 0 };
//     uint8 line_detect_center = 0;//中线数组的有效点
//     uint8 line_detect_left = 0;//中线数组的有效点
//     uint8 line_detect_right = 0;//中线数组的有效点
//     uint8 l_maxcol[2]={0};//从左边开始寻找到的最长白烈，更靠近右边 0是高度 1是位置
//     uint8 r_maxcol[2]={0};
//     /*
//      * 函数作用:寻线函数
//      * 入口参数:无
//      * 使用示例：processor.find_line();
//      * 备注: 寻线的主逻辑函数,所有寻线函数在此处调用
//      */
//     void find_line();//寻迹
//     /*
//      * 函数作用:用sobel算子获得八邻域开始点
//      * 入口参数:找点的灰度图像
//      * 使用示例：processor.get_start_point_sobel();
//      * 备注: 适合灰度八领域
//      */
//     uint8 get_start_point_sobel(const cv::Mat& img);
//     /*
//      * 函数作用:寻找二值化跳变点获得八邻域开始点
//      * 入口参数:找点的灰度图像
//      * 使用示例：processor.get_start_point();
//      * 备注: 适合二值化八领域
//      */
//     uint8_t get_start_point(const cv::Mat& bin_image, int start_row);
//     /*寻线*/
//
//     /*绘图*/
//     /*
//      * 函数作用:在图像上画出边线
//      * 入口参数:绘制的彩色图像 边线数组 颜色 绘制的点数
//      * 使用示例：processor.draw_line_1(processor.disp_color_image,processor.l_border,cv::Scalar(255, 0, 0), image_h - 1);
//      * 备注:
//      */
//     void draw_line_1(cv::Mat& image, const uint8_t border[], cv::Scalar color, int num);
//     /*绘图*/
//
//     /*调试信息*/
//     uint16 points_test[4][2];
//     /*调试信息*/
//
//     /*二值化*/
//     /*二值化*/
//
//     /*颜色查找*/
//     //颜色阈值
//     std::array<short, 6> green_hsv_th = {0, 250, 0, 250, 0, 250};
//     std::array<short, 6> red_hsv_th = {0, 250, 0, 250, 0, 250};
//     std::array<short, 6> blue_hsv_th = {0, 250, 0, 250, 0, 250};
//
//     /*
//      * 函数作用:修改hsv_th数组的值
//      * 入口参数:要修改的hsv_th标志位,要修改的hsv的上限,hsv的下限
//      * 使用示例：imgProcessor.change_color_hsv("green",85,255,255,35,100,100);
//      * 备注:
//      */
//     bool change_color_hsv(const std::string& change_flag,short h_max,short s_max,short v_max,short h_min,short s_min,short v_min);
//     /*
//      * 函数作用:寻找最大色块
//      * 入口参数:输入图像,hsv_th的值
//      * 返回值:找到的最大色块的中心坐标
//      * 备注:函数内有色块面积的限幅
//      */
//     std::array<float,2> findLargestColoContour(const cv::Mat& image,std::array<short,6> hsv_th);
//     /*颜色查找*/
//
//
//     float Det_True;
//     void odemory_start(uint8 use_odemory);
//     void odemory_get(uint8 use_odemory);
//     void odemory_stop(uint8 use_odemory);
//
//
//     /*初始化*/
//     cv::Mat mt9v03x_image;//使用的灰度图像
//     cv::Mat disp_color_image;//显示的彩色图像
//     /*
//      * 函数作用:加载图像进入类
//      * 入口参数:输入的灰度图像
//      * 使用示例：processor.setImage(color_image);
//      * 备注:
//      */
//     void setImage(const cv::Mat& inputImage);
//     void cube_recognize(const cv::Mat &inputImage);
//     img_process_object();
//     /*初始化*/
//
// private:
//
//     /*颜色查找*/
//     //查找色块后获得的点的坐标位置
//     std::array<short, 6> positon_red = {0, 0};
//     std::array<short, 6> positon_blue = {0, 0};
//     std::array<short, 6> positon_green = {0, 0};
//
//     /*颜色查找*/
//
//     /*逆透视*/
//     // double change_un_Mat[3][3] ={{-1.562177,1.954244,-149.595835},{-0.049606,0.867789,-152.197266},{-0.000094,0.023116,-3.217693}};
//     cv::Mat inverseMatrix = (cv::Mat_<double>(3, 3) <<
//     -1.734241,2.162499,-118.147096,
//     0.027528,0.978298,-158.015057,
//     0.000459,0.025404,-3.070356);
//     /*逆透视*/
//
//     /*里程计*/
//     bool odemory_flag0 = false;
//     bool odemory_flag1 = false;
//     bool odemory_flag2 = false;
//     bool odemory_flag3 = false;
//     /*里程计*/
//
//     /*元素*/
//     void Element_Detection(const cv::Mat& inputImage);//元素检测
//     void Staight_Detection(const cv::Mat& inputImage);
//     void cross_fill(const cv::Mat &inputImage);
//     void circle_fill_right_qlh(const cv::Mat &inputImage);
//     void circle_fill_left_qlh(const cv::Mat &inputImage);
//     /*元素*/
//
//     /*前瞻*/
//     void Error_offline();
//
//     float DetQueue[20] = {0};      // 储存误差
//     int det_index = 0;
//     const int DET_QUEUE_SIZE = 20;
//     const int DET_DIFF_INTERVAL = 6;
//
//     float DetQueue1[20] = {0};      // 储存误差
//     int det_index1 = 0;
//     const int DET_QUEUE_SIZE1 = 20;
//     const int DET_DIFF_INTERVAL1 = 6;
//     /*前瞻*/
//
//     /*寻线*/
//
//     int sobel_th = 7; //sobel 阈值
//     uint8 start_cow_l = use_w/2;//sobel算子左边寻线开始行
//     uint8 start_cow_r = use_w/2;
//     uint8 start_point_l[2] = { 0 };//八领域种子生长开始的点
//     uint8 start_point_r[2] = { 0 };
//     uint8 hightest = 0;//八领域图像最高点
//
//
//
//
//     /*
//     * 函数作用:计算当前点的sobel值
//     * 入口参数:输入图像 x坐标 y坐标
//     * 使用示例：calculateSobelAtPixel(img, col, start_row)
//     * 备注: 用来计算当前sobel值并与sobel阈值比较可以判断是否为边线
//     */
//     int calculateSobelAtPixel(const cv::Mat& src, int x, int y);
//     /*
//     * 函数作用:二值化八领域算法
//     * 入口参数:最多计算多少个点,使用的图像,左起始点的x,y,右起始点的x,y，图像的最高点
//     * 使用示例：search_l_r((uint16)USE_num, temp, start_point_l[0], start_point_l[1],
//                    start_point_r[0], start_point_r[1], &hightest);
//     * 备注: 用来计算当前sobel值并与sobel阈值比较可以判断是否为边线
//     */
//     void search_l_r(
//     uint16_t break_flag,
//     const cv::Mat& image,
//     uint8_t l_start_x,
//     uint8_t l_start_y,
//     uint8_t r_start_x,
//     uint8_t r_start_y,
//     uint8_t* hightest
// );
//     void vgetborder_maxwhitecols(const cv::Mat &inputImage);
//     /*
//     * 函数作用:从八领域提取左边线
//     * 入口参数:左边线一共有多少点
//     * 使用示例：get_left(data_stastics_l);
//     * 备注: 在find_line中调用
//     */
//     void get_left(uint16 total_L);//提取左边线
//     void left_get(uint16 total_L);//提取左边线
//     /*
//     * 函数作用:从八领域提取右边线
//     * 入口参数右边线一共有多少点
//     * 使用示例：get_right(data_stastics_r);
//     * 备注: 在find_line中调用
//     */
//     void get_right(uint16 total_R);//提取右边线
//     void right_get(uint16 total_R);//提取右边线
//
//     /*
//      * 函数作用:根据不同的左右边线获取中线
//      * 入口参数:无
//      * 使用示例：vget_center_shangjiao();
//      * 备注:在find_line中调用
//      */
//     void vget_center_shangjiao();
//
//     /*寻线*/
//
//     /*二值化*/
//     typedef int otsuTh_eight_type;
//     typedef uint8_t make_sizetype;
//     typedef int Threshold_min_type;
//
//     //局部OSTU
//     uint8_t break_flag = 200;
//     make_sizetype make_size = 3;//卷积核大小
//     Threshold_min_type Threshold_min = 12;//阈值
//     /*
//      * 函数作用:局部大津法
//      * 入口参数:算子大小 输入图像 运算点的x,y 最低像素值
//      * 使用示例：
//      * 备注:超出边界的部分认为是黑点
//      */
//     static otsuTh_eight_type otsuThreshold_average(make_sizetype make_size, const cv::Mat& img, uint16_t x, uint16_t y, Threshold_min_type Threshold_min);
//     /*
//      * 函数作用:优化后的大津法
//      * 入口参数:输入图像 阈值
//      * 使用示例：
//      * 备注:
//      */
//     uint8_t Threshold_deal(const cv::Mat& image, uint32_t pixel_threshold);//优化后的大津法
//     /*
//     * 函数作用:分区域进行大津法
//     * 入口参数:输入图像 输出图像 静态阈值 大津法阈值
//     * 使用示例：
//     * 备注:
//     */
//     void Get01change_dajin(const cv::Mat& inputImage, cv::Mat& outputImage, int threshold_static, int threshold_detach);
//     /*
//     * 函数作用:分区域进行二值化
//     * 入口参数:输入图像 输出图像 静态阈值
//     * 使用示例：
//     * 备注:观察计算出静态阈值
//     */
//     void Get01change(const cv::Mat& inputImage, cv::Mat& outputImage, int thresholdStatic);
//     /*二值化*/
//
//     /*预处理*/
//     void image_draw_rectan(cv::Mat& image);
//     /*预处理*/
//
//     /*调试信息*/
//     void print_average_huidu(const cv::Mat& img, make_sizetype make_size, Threshold_min_type Threshold_min);
//     /*调试信息*/
//
//     /*数学工具*/
//     float Slope_Calculate(uint8 begin, uint8 end, uint8 *border);
//     void calculate_s_i(uint8 start, uint8 end, uint8 *border, float *slope_rate, float *intercept);
//     void CircleAddingLine(uint8 *l_border, uint8 *r_border,uint8_t status, uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY);
//     #define limit_a_b(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))
//     void fingdoubleright();
//     void fingdoubleleft();
//     uint8 LEFT_just;
//     uint8 RIGHT_just;
//     void LINE_JUST(uint8 staus);
//     int Lose_line_L(uint8 *l_border);
//     int Lose_line_R(uint8 *r_border);
//     void zebra_crossing(const cv::Mat &inputImage);
//     void podao(const cv::Mat &inputImage);
//     void obstacle(const cv::Mat &inputImage);
//
//     int My_Abs(int a, int b);
//      /*数学工具*/
//
//     //圆环参数
//     bool truedoubleright=false;
//     bool truedoubleleft=false;
//
//
//     /*接口类*/
//
//
//     uint8 get_prospect();//计算当前前瞻点
//     /*接口类*/
// };
// img_process_object::img_process_object(): mt9v03x_image(USED_ROW, USED_COL, CV_8UC1, cv::Scalar(0))  {
//
// }
//
// /*
//  * 函数作用:修改hsv_th的值
//  * 入口参数:要修改的对应颜色的标志位,h_max.s_max,v_max,h_min,s_min,v_min
//  */
// bool img_process_object::change_color_hsv(const std::string& change_flag, short h_max, short s_max, short v_max, short h_min, short s_min, short v_min)
// {
//     if (change_flag == "red") {
//         red_hsv_th[0] = h_max;
//         red_hsv_th[1] = s_max;
//         red_hsv_th[2] = v_max;
//         red_hsv_th[3] = h_min;
//         red_hsv_th[4] = s_min;
//         red_hsv_th[5] = v_min;
//     }
//     else if (change_flag == "green") {
//         green_hsv_th[0] = h_max;
//         green_hsv_th[1] = s_max;
//         green_hsv_th[2] = v_max;
//         green_hsv_th[3] = h_min;
//         green_hsv_th[4] = s_min;
//         green_hsv_th[5] = v_min;
//     }
//     else if (change_flag == "blue") {
//         blue_hsv_th[0] = h_max;
//         blue_hsv_th[1] = s_max;
//         blue_hsv_th[2] = v_max;
//         blue_hsv_th[3] = h_min;
//         blue_hsv_th[4] = s_min;
//         blue_hsv_th[5] = v_min;
//     }
//     else {
// //        ROS_WARN("fall color flag: %s", change_flag.c_str());
//         return false;  // 无效标志，返回 false
//     }
//
//     return true;  // 成功修改
// }
// std::array<float,2> img_process_object::findLargestColoContour(const cv::Mat& image, std::array<short, 6> hsv_th)
// {
//     cv::Mat hsvImage;
//     cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);
//
//     // 定义绿色的阈值范围
//     cv::Scalar upperColor(hsv_th[0], hsv_th[1], hsv_th[2]); // 上界
//     cv::Scalar lowerColor(hsv_th[3], hsv_th[4], hsv_th[5]); // 下界
//
//     // 根据阈值创建掩膜
//     cv::Mat mask;
//     cv::inRange(hsvImage, lowerColor, upperColor, mask);
//
//     // 查找轮廓
//     std::vector<std::vector<cv::Point>> contours;
//     cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//
//     double maxArea = 0;
//     cv::Rect largestRect;
//
//     // 检查是否找到轮廓
//     if (contours.empty()) {
// //        ROS_INFO("NO CONTOURS");
//         return {-1, -1};  // 返回无效坐标
//     }
//
//     for (const auto& cnt : contours) {
//         double area = cv::contourArea(cnt);
//         if (area > maxArea) {
//             maxArea = area;
//             largestRect = cv::boundingRect(cnt);  // 更新最大轮廓的边界矩形
// //            ROS_INFO("find counter,area=(%f)", area);
//         }
//     }
//     int areaThreshold_min = 0;
//     int areaThreshold_max = 20000;
//     // 检查最大矩形的面积是否有效
//     if (maxArea < areaThreshold_min || maxArea > areaThreshold_max) {
// //        ROS_INFO("least(%f) < th (%d)，or >biggest(%d).", maxArea, areaThreshold_min,areaThreshold_max);
//         return {-1, -1};  // 返回无效坐标
//     }
//     cv::rectangle(image, largestRect, cv::Scalar(255, 0, 0), 2);  // 使用绿色矩形框标记
//     //cv::imshow("a",image);
//     //cv::waitKey(1);
//     float centerX = largestRect.x + largestRect.width / 2.0;
//     float centerY = largestRect.y + largestRect.height / 2.0;
//     //ROS_INFO("最大轮廓x=%f,y=%f:", centerX,centerY);
//     return {centerX, centerY};  // 返回中心坐标
// }
//
//
// cv::Mat *PerImg_ip[RESULT_ROW][RESULT_COL];
//
//
// void img_process_object::setImage(const cv::Mat& inputImage) {
//     // 检查输入图像的尺寸和类型是否匹配
//     if (inputImage.rows != USED_ROW || inputImage.cols != USED_COL || inputImage.type() != CV_8UC1) {
//         throw std::invalid_argument("Input image size or type does not match mt9v03x_image");
//     }
//
//     // 复制图像内容到 mt9v03x_image
//     inputImage.copyTo(mt9v03x_image);
// }
//
//
//
// int img_process_object::calculateSobelAtPixel(const cv::Mat& img, int x, int y) {
//     if (img.empty() || img.type() != CV_8UC1) {
//         throw std::invalid_argument("Input image must be a non-empty grayscale image.");
//     }
//
//     // 检查坐标是否在有效范围内
//     if (x <= 0 || x >= img.cols - 1 || y <= 0 || y >= img.rows - 1) {
//         throw std::out_of_range("Pixel coordinates must be within valid range.");
//     }
//
//     // Sobel 核计算
//     int gx = (-1 * img.at<uchar>(y - 1, x - 1) + 1 * img.at<uchar>(y - 1, x + 1) +
//               -2 * img.at<uchar>(y, x - 1) + 2 * img.at<uchar>(y, x + 1) +
//               -1 * img.at<uchar>(y + 1, x - 1) + 1 * img.at<uchar>(y + 1, x + 1)) / 4;
//
//     int gy = (1 * img.at<uchar>(y - 1, x - 1) + 2 * img.at<uchar>(y - 1, x) + 1 * img.at<uchar>(y - 1, x + 1) +
//               -1 * img.at<uchar>(y + 1, x - 1) - 2 * img.at<uchar>(y + 1, x) - 1 * img.at<uchar>(y + 1, x + 1)) / 4;
//
//     // 梯度幅值计算
//     float sobel = (std::abs(gx) + std::abs(gy)) / 2.0f;
//     return sobel;
// }
//
// uint8_t img_process_object::get_start_point(const cv::Mat& bin_image, int start_row) {
//
//     start_point_l[0] = 0;//x
//     start_point_l[1] = 0;//y
//
//     start_point_r[0] = 0;//x
//     start_point_r[1] = 0;//y
//     bool l_found = false, r_found = false;
//
//     // 边界检查
//     if (start_row < 0 || start_row >= RESULT_ROW) {
//         return 0; // 起始行超出图像范围
//     }
//     // 从中间向左寻找
//     for (int i = RESULT_COL / 2; i > border_min; --i) {
//         if (bin_image.at<uint8_t>(start_row, i) == 255 && bin_image.at<uint8_t>(start_row, i - 1) != 255) {
//             start_point_l[0] = i;//x
//             start_point_l[1] = start_row;
//             l_found = true;
//             break;
//         }
//     }
//
//     // 从中间向右寻找
//     for (int i = RESULT_COL / 2; i < RESULT_COL-1; ++i) {
//         if (bin_image.at<uint8_t>(start_row, i) == 255 && bin_image.at<uint8_t>(start_row, i + 1) != 255) {
//             start_point_r[0] = i;//x
//             start_point_r[1] = start_row;
//             r_found = true;
//             break;
//         }
//     }
//
//     // 输出结果
//     if (l_found && r_found) {
//
//         return 1;
//     } else {
//         return 0;
//     }
// }
//
// inline uint8 img_process_object::get_start_point_sobel(const cv::Mat& img) {
//     uint8 l_found = 0, r_found = 0;
//
//     // 初始化起始点
//     start_point_l[0] = 0; // x
//     start_point_l[1] = 0; // y
//     start_point_r[0] = 0; // x
//     start_point_r[1] = 0; // y
//
//
//     uint8 start_row = start_h;
//
//
//         // 查找左边起始点
//         for (int col = start_cow_l; col > border_min - 1; col--) {
//             if (calculateSobelAtPixel(img, col, start_row) > sobel_th) {
//                 start_point_l[0] = col; // x
//                 start_point_l[1] = start_row; // y
//                 l_found = 1;
//
//                 break;
//             }
//         }
//
//         // 查找右边起始点
//         for (int col = start_cow_r; col < border_max - 1; col++) {
//             if (calculateSobelAtPixel(img, col, start_row) > sobel_th) {
//                 start_point_r[0] = col; // x
//                 start_point_r[1] = start_row; // y
//                 r_found = 1;
//
//                 break;
//             }
//         }
//
//         // 如果左右点都找到，则返回成功
//         if (l_found && r_found) {
//             if (start_point_l[0] + 10 >72 || start_cow_r <72) {
//                 start_cow_l = use_w/2;
//                 start_cow_r = use_w/2;
//             }
//             else
//             {
//                 start_cow_l = start_point_l[0] + 40 ;
//                 start_cow_r = start_point_r[0] - 40 ;
//             }
//
//             return 1;
//         } else {
//             l_found = 0;
//             r_found = 0;
//             return 0;
//         }
//     // }
//
//     // 如果未找到起始点，返回失败
//     return 0;
// }
// // 逆透视：对给定边线的每个点 (x, y)，应用 3x3 矩阵，返回原图坐标的 x 值到 transformed_x 数组中
// // border:       原始边线，按 y 存储 x 坐标（大小为 use_h）
// // transformed_x: 返回变换后的 x 坐标（大小为 use_h）
// // use_h:        图像高度（行数）
// // inverseMat:   3x3 的逆透视矩阵
//
// cv::Mat map_x, map_y;
// bool map_initialized = false;
// inline cv::Mat img_process_object::inversePerspectiveTransform(const cv::Mat& src, const cv::Mat& inverseMatrix, int result_rows, int result_cols){
//     CV_Assert(src.channels() == 1);
//     CV_Assert(inverseMatrix.rows == 3 && inverseMatrix.cols == 3 && inverseMatrix.type() == CV_64F);
//
//     if (!map_initialized) {
//         // 构造目标图像的齐次坐标矩阵
//         cv::Mat dst_homo(3, result_rows * result_cols, CV_64F);
//         for (int y = 0; y < result_rows; ++y) {
//             for (int x = 0; x < result_cols; ++x) {
//                 int idx = y * result_cols + x;
//                 dst_homo.at<double>(0, idx) = x;
//                 dst_homo.at<double>(1, idx) = y;
//                 dst_homo.at<double>(2, idx) = 1.0;
//             }
//         }
//
//         // 逆透视映射
//         cv::Mat mapped = inverseMatrix * dst_homo;
//
//         // 归一化
//         for (int i = 0; i < mapped.cols; ++i) {
//             double w = mapped.at<double>(2, i);
//             if (w != 0.0) {
//                 mapped.at<double>(0, i) /= w;
//                 mapped.at<double>(1, i) /= w;
//             }
//         }
//
//         // 初始化映射图 map_x 和 map_y
//         map_x.create(result_rows, result_cols, CV_32F);
//         map_y.create(result_rows, result_cols, CV_32F);
//         for (int i = 0; i < result_rows; ++i) {
//             for (int j = 0; j < result_cols; ++j) {
//                 int idx = i * result_cols + j;
//                 map_x.at<float>(i, j) = static_cast<float>(mapped.at<double>(0, idx));
//                 map_y.at<float>(i, j) = static_cast<float>(mapped.at<double>(1, idx));
//             }
//         }
//
//         map_initialized = true;
//     }
//
//     // 直接使用缓存的 map 进行 remap
//     cv::Mat dst;
//     cv::remap(src, dst, map_x, map_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT, 0);
//     return dst;
// }
// /*
// 函数名称：void image_draw_rectan(cv::Mat& image)
// 功能说明：给图像画一个黑框
// 参数说明：cv::Mat& image 图像对象（灰度图或单通道图像）
// 函数返回：无
// 修改时间：2025年1月10日
// 备    注：
// example： image_draw_rectan(image);
// */
// inline void img_process_object::image_draw_rectan(cv::Mat& image) {
//     // 检查图像是否为空
//     if (image.empty()) {
//         throw std::invalid_argument("Input image is empty.");
//     }
//
//     // 检查图像是否为单通道
//     if (image.channels() != 1) {
//         throw std::invalid_argument("Input image must be a single-channel grayscale image.");
//     }
//
//     int get_image_h = image.rows; // 图像高度
//     int get_image_w = image.cols; // 图像宽度
//
//     // 绘制左右两列的黑框
//     for (int i = 0; i < get_image_h; i++) {
//         image.at<uchar>(i, 0) = 0;       // 第一列
//         image.at<uchar>(i, 1) = 0;       // 第二列
//         image.at<uchar>(i, get_image_w - 1) = 0; // 最后一列
//         image.at<uchar>(i, get_image_w - 2) = 0; // 倒数第二列
//     }
//
//     // 绘制上下两行的黑框
//     for (int i = 0; i < get_image_w; i++) {
//         image.at<uchar>(0, i) = 0;       // 第一行
//         image.at<uchar>(1, i) = 0;       // 第二行
//         // image.at<uchar>(image_h - 1, i) = 0; // 最后一行（如果需要，可以解注释）
//     }
// }
//
// inline void img_process_object::print_average_huidu(const cv::Mat& img, make_sizetype make_size, Threshold_min_type Threshold_min) {
//     // 检查图像是否有效
//     if (img.empty() || img.channels() != 1) {
//         throw std::invalid_argument("Input image must be a non-empty grayscale image.");
//     }
//
//     // 遍历图像的每个像素点
//     for (int y = make_size / 2; y < img.rows - make_size / 2; y++) {
//         for (int x = make_size / 2; x < img.cols - make_size / 2; x++) {
//             // 调用 otsuThreshold_average 函数计算平均灰度值
//             int average_huidu = otsuThreshold_average(make_size, img, x, y, Threshold_min);
//
//         }
//     }
// }
//
// inline float img_process_object::Slope_Calculate(uint8 begin, uint8 end, uint8 *border) {
//     float xsum = 0, ysum = 0, xysum = 0, x2sum = 0;
// 	int16 i = 0;
// 	float result = 0;
// 	static float resultlast;
//
// 	for (i = begin; i < end; i++)
// 	{
// 		xsum += i;
// 		ysum += border[i];
// 		xysum += i * (border[i]);
// 		x2sum += i * i;
//
// 	}
// 	if ((end - begin)*x2sum - xsum * xsum) //判断除数是否为零
// 	{   //最小二乘法计算斜率
// 		result = ((end - begin)*xysum - xsum * ysum) / ((end - begin)*x2sum - xsum * xsum);
// 		resultlast = result;
// 	}
// 	else
// 	{
// 		result = resultlast;
// 	}
// 	return result;
// }
//
// inline void img_process_object::calculate_s_i(uint8 start, uint8 end, uint8 *border, float *slope_rate,
//     float *intercept) {
//     if (start >= end) { // 检查范围有效性
//         *slope_rate = 0;
//         *intercept = 0;
//         return;
//     }
//
//     uint16 xsum = 0, ysum = 0;
//     uint16 num = end - start;
//
//     // 计算 xsum 和 ysum
//     for (uint16 i = start; i < end; i++) {
//         xsum += i;
//         ysum += border[i];
//     }
//
//     // 计算平均值
//     float x_average = (float)xsum / num;
//     float y_average = (float)ysum / num;
//
//     // 计算斜率和截距
//     *slope_rate = Slope_Calculate(start, end, border); // 确保 Slope_Calculate 的实现有效 k
//     *intercept = y_average - (*slope_rate) * x_average;// b
//
// }
//
// inline int img_process_object::otsuThreshold_average(make_sizetype make_size, const cv::Mat& img, uint16_t x, uint16_t y, Threshold_min_type Threshold_min) {
//     // 检查图像是否有效
//     if (img.empty() || img.channels() != 1) {
//         throw std::invalid_argument("Input image must be a non-empty grayscale image.");
//     }
//
//     // 计算邻域内的平均灰度值
//     make_sizetype half = make_size / 2;
//     otsuTh_eight_type average_huidu = 0;
//     int valid_pixel_count = 0; // 有效像素点计数
//
//     for (int dy = -half; dy <= half; dy++) {
//         for (int dx = -half; dx <= half; dx++) {
//             int nx = x + dx; // 邻域内像素点的 x 坐标
//             int ny = y + dy; // 邻域内像素点的 y 坐标
//
//             // std::cout << static_cast<int>(img.at<uchar>(111, 3)) << " ";
//             // 检查是否在图像范围内
//             if (nx >= 0 && nx < img.cols && ny >= 0 && ny < img.rows) {
//
//                 average_huidu += static_cast<int>(img.at<uchar>(111, 3)); // 使用 uchar 访问像素
//
//             }
//             else {
//
//                 average_huidu += 255; // 超出边界的像素值设为 255
//
//             }
//         }
//     }
//     average_huidu = average_huidu / (make_size*make_size);
//     average_huidu -= Threshold_min;
//
//     //
//
//     return average_huidu;
// }
// inline void img_process_object::vgetborder_maxwhitecols(const cv::Mat &inputImage)
// {
//     int White_Column[use_w] = {0}; //每列白列长度
//     //获取白色直方图
//     for (int lie =2; lie<RESULT_COL-3; lie++)
//     {
//         for (int hang = RESULT_ROW - 1; hang > offline; hang--)
//         {
//             if (inputImage.at<uint8_t>(hang, lie)!=255)
//             {
//                 // std::cout << "第 " << hang << " 行遇到黑色，中断扫描" << std::endl;
//                 break;
//             }
//             else
//             {
//                 White_Column[lie]++;
//             }
//         }
//     }
//
//     //获取最长白列位置
//     unsigned char ucl_hightestcol[2]={0},ucr_hightestcol[2]={0};
//     for (int i=60;i<99;i++)//宽度
//     {
//         if (ucr_hightestcol[0]<White_Column[i]) {
//             ucr_hightestcol[0]=White_Column[i];//0是长度
//             ucr_hightestcol[1]=i;//1是位置
//         }
//
//     }
//
//     for (int i=99;i>60;i--)//宽度
//     {
//         if (ucl_hightestcol[0]<White_Column[i]) {
//             ucl_hightestcol[0]=White_Column[i];//0是长度
//             ucl_hightestcol[1]=i;//1是位置
//         }
//     }
//     ucr_hightestcol[0]=RESULT_ROW-1-ucr_hightestcol[0];
//     ucl_hightestcol[0]=RESULT_ROW-1-ucl_hightestcol[0];
//     line_detect_left=ucl_hightestcol[0];
//     line_detect_right=ucr_hightestcol[1];
//     line_detect_center = max(ucl_hightestcol[0], ucr_hightestcol[0]); // 获取中心行
//
//
//
//         //
//     r_border_lostnum=0;
//     l_border_lostnum=0;
//     for (int i=RESULT_ROW-1;i>=line_detect_center;i--)
//     {
//         for (int j=ucr_hightestcol[1];j<=RESULT_COL-1-2;j++)
//         {
//             if (inputImage.at<uint8_t>(i, j)==255&&inputImage.at<uint8_t>(i, j+1)!=255&&inputImage.at<uint8_t>(i, j+2)!=255) {
//                 r_border[i]=j;
//                 //not lost
//                 // printf("r_border[i]=%d\n",r_border[i]);
//                 break;
//             }
//             else if (j>=rlost_border||j<60) {
//                 r_border[i]=255;
//                 r_border_lostnum++;//lost
//
//
//                 break;
//             }
//         }
//         for (int j=ucl_hightestcol[1];j>=2;j--)
//         {
//             if (inputImage.at<uint8_t>(i, j)==255&&inputImage.at<uint8_t>(i, j-1)!=255&&inputImage.at<uint8_t>(i, j-2)!=255) {
//                 l_border[i]=j;
//                 // not
//                 // printf("l_border[i]=%d\n",l_border[i]);
//                 break;
//             }
//             else if (j<llost_border||j>=100) {
//                 l_border[i]=255;
//                 l_border_lostnum++;//lost
//
//                 break;
//             }
//         }
//
//
//         l_maxcol[1]=ucl_hightestcol[1];//1是位置
//         l_maxcol[0]=ucl_hightestcol[0];//0是长度
//         r_maxcol[1]=ucr_hightestcol[1];
//         r_maxcol[0]=ucr_hightestcol[0];
//
//     }
//
// }
// inline void img_process_object::search_l_r(
//     uint16_t break_flag,
//     const cv::Mat& image,
//     uint8_t l_start_x,
//     uint8_t l_start_y,
//     uint8_t r_start_x,
//     uint8_t r_start_y,
//     uint8_t* hightest
// )
// {
//     uint8 i = 0, j = 0;
//
//     //左边变量
//     uint8 search_filds_l[8][2] = { {  0 } };
//     uint8 index_l = 0;
//     uint8 temp_l[8][2] = { {  0 } };
//     uint8 center_point_l[2] = { 0 };
//     uint16 l_data_statics;//统计左边
//     //定义八个邻域
//     static int8 seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,  0},{1, 1}, };
//     //{-1,-1},{0,-1},{+1,-1},
//     //{-1, 0},       {+1, 0},
//     //{-1,+1},{0,+1},{+1,+1},
//     //这个是顺时针
//
//     //右边变量
//     uint8 search_filds_r[8][2] = { {  0 } };
//     uint8 center_point_r[2] = { 0 };//中心坐标点
//     uint8 index_r = 0;//索引下标
//     uint8 temp_r[8][2] = { {  0 } };
//     uint16 r_data_statics;//统计右边
//     //定义八个邻域
//     static int8 seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}, };
//     //{-1,-1},{0,-1},{+1,-1},
//     //{-1, 0},       {+1, 0},
//     //{-1,+1},{0,+1},{+1,+1},
//     //这个是逆时针
//
//     l_data_statics = data_stastics_l;//统计找到了多少个点，方便后续把点全部画出来
//     r_data_statics = data_stastics_r;//统计找到了多少个点，方便后续把点全部画出来
//
//     //第一次更新坐标点  将找到的起点值传进来
//     center_point_l[0] = l_start_x;//x
//
//     center_point_l[1] = l_start_y;//y
//     center_point_r[0] = r_start_x;//x
//     center_point_r[1] = r_start_y;//y
//
//     //开启邻域循环
//     while (break_flag--)
//     {
//
//         //左边
//         for (i = 0; i < 8; i++)//传递8F坐标
//         {
//             search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];//x
//             search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];//y
//         }
//         //中心坐标点填充到已经找到的点内
//         points_l[l_data_statics][0] = center_point_l[0];//x
//         points_l[l_data_statics][1] = center_point_l[1];//y
//
//         l_data_statics++;//索引加一
//
//         //右边
//         for (i = 0; i < 8; i++)//传递8F坐标
//         {
//             search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
//             search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
//         }
//         //中心坐标点填充到已经找到的点内
//         points_r[r_data_statics][0] = center_point_r[0];//x
//         points_r[r_data_statics][1] = center_point_r[1];//y
//
//         index_l = 0;//先清零，后使用
//         for (i = 0; i < 8; i++)
//         {
//             temp_l[i][0] = 0;//先清零，后使用
//             temp_l[i][1] = 0;//先清零，后使用
//         }
//
//         //左边判断
//         for (i = 0; i < 8; i++)
//         {
//
//
//             if (image.at<uint8_t>(search_filds_l[i][1], search_filds_l[i][0])!= 255
//                 && image.at<uint8_t>(search_filds_l[(i + 1) & 7][1],search_filds_l[(i + 1) & 7][0]) == 255)
//             {
//                 temp_l[index_l][0] = search_filds_l[(i)][0];
//                 temp_l[index_l][1] = search_filds_l[(i)][1];
//
//                 index_l++;
//                 dir_l[l_data_statics - 1] = (i);//记录生长方向
//             }
//
//             if (index_l)
//             {
//                 //更新坐标点
//                 center_point_l[0] = temp_l[0][0];//x
//                 center_point_l[1] = temp_l[0][1];//y
//
//                 for (j = 0; j < index_l; j++)
//                 {
//                     if (center_point_l[1] > temp_l[j][1])
//                     {
//                         center_point_l[0] = temp_l[j][0];//x
//                         center_point_l[1] = temp_l[j][1];//y
//                     }
//                 }
//             }
//
//         }
//         // printf("temp_l[0] = %d, temp_l[1] = %d\n", temp_l[0][0], temp_l[0][1]);
//         // printf("points_l[l_data_statics - 1]=%d %d,%d,%d,%d,%d\n",points_l[l_data_statics - 1][0],points_l[l_data_statics - 1][1],points_l[l_data_statics - 2][0],points_l[l_data_statics - 2][1],points_l[l_data_statics - 3][0],points_l[l_data_statics - 3][1]);
//         if ((points_r[r_data_statics][0] == points_r[r_data_statics - 1][0] && points_r[r_data_statics][0] == points_r[r_data_statics - 2][0]
//             && points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1])
//             || (points_l[l_data_statics - 1][0] == points_l[l_data_statics - 2][0] && points_l[l_data_statics - 1][0] == points_l[l_data_statics - 3][0]
//                 && points_l[l_data_statics - 1][1] == points_l[l_data_statics - 2][1] && points_l[l_data_statics - 1][1] == points_l[l_data_statics - 3][1]))
//         {
//             //printf("三次进入同一个点，退出\n");
//             break;
//         }
//         if (abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2
//             && abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1] < 2)
//             )
//         {
//             //printf("\n左右相遇退出\n");
//             *hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;//取出最高点
//             //printf("\n在y=%d处退出\n",*hightest);
//             break;
//         }
//         if ((points_r[r_data_statics][1] < points_l[l_data_statics - 1][1]))
//         {
//             // printf("\n如果左边比右边高了，左边等待右边\n");
//             continue;//如果左边比右边高了，左边等待右边
//         }
//         if (dir_l[l_data_statics - 1] == 7
//             && (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1]))//左边比右边高且已经向下生长了
//         {
//             //printf("\n左边开始向下了，等待右边，等待中... \n");
//             center_point_l[0] = (uint8)points_l[l_data_statics - 1][0];//x
//             center_point_l[1] = (uint8)points_l[l_data_statics - 1][1];//y
//             l_data_statics--;
//         }
//         r_data_statics++;//索引加一
//
//         index_r = 0;//先清零，后使用
//         for (i = 0; i < 8; i++)
//         {
//             temp_r[i][0] = 0;//先清零，后使用
//             temp_r[i][1] = 0;//先清零，后使用
//         }
//
//         //右边判断
//         for (i = 0; i < 8; i++)
//         {
//             if (image.at<uint8_t>(search_filds_r[i][1],search_filds_r[i][0]) != 255
//                 && image.at<uint8_t>(search_filds_r[(i + 1) & 7][1],search_filds_r[(i + 1) & 7][0]) == 255)
//             {
//                 temp_r[index_r][0] = search_filds_r[(i)][0];
//                 temp_r[index_r][1] = search_filds_r[(i)][1];
//                 // points_test[1][0]=search_filds_r[(i)][0];
//                 // points_test[1][1]=search_filds_r[(i)][1];
//                 index_r++;//索引加一
//                 dir_r[r_data_statics - 1] = (i);//记录生长方向
//
//                 //printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
//             }
//             if (index_r)
//             {
//
//                 //更新坐标点
//                 center_point_r[0] = temp_r[0][0];//x
//                 center_point_r[1] = temp_r[0][1];//y
//
//                 for (j = 0; j < index_r; j++)
//                 {
//                     if (center_point_r[1] > temp_r[j][1])
//                     {
//                         center_point_r[0] = temp_r[j][0];//x
//                         center_point_r[1] = temp_r[j][1];//y
//                     }
//                     // printf("temp_r[0] = %d, temp_r[1] = %d\n", temp_r[0][0], temp_r[0][1]);
//                 }
//
//             }
//         }
//
//
//     }
//
//
//     //取出循环次数
//
//     data_stastics_l = l_data_statics;
//     data_stastics_r = r_data_statics;
// }
//
// inline void img_process_object::get_left(uint16 total_L)
// {
//     uint8 i = 0;
//     uint16 j = 0;
//     uint8 h = 0;
//
//     //初始化
//     for (i = 0; i < RESULT_ROW; i++)
//     {
//         l_border[i] = 255;
//     }
//     h = start_h;
//
//     //左边
//     for (j = 0; j < total_L; j++)
//     {
//
//         if (points_l[j][1] == h)
//         {
//
//             l_border[h] = points_l[j][0] + 1;
//
//         }
//         else continue; //每行只取一个点，没到下一行就不记录
//
//         h--;
//         if (h == 0)
//         {
//             break;//到最后一行退出
//         }
//     }
//
// }
//
// inline void img_process_object::get_right(uint16 total_R)
// {
//     uint8 i = 0;
//     uint16 j = 0;
//     uint8 h = 0;
//     for (i = 0; i < RESULT_ROW; i++)
//     {
//         r_border[i] = 255;//右边线初始化放到最右边，左边线放到最左边，这样八邻域闭合区域外的中线就会在中间，不会干扰得到的数据
//     }
//     h = start_h;
//     //右边
//     for (j = 0; j < total_R; j++)
//     {
//         if (points_r[j][1] == h)
//         {
//             r_border[h] = points_r[j][0] - 1;
//
//         }
//         else continue;//每行只取一个点，没到下一行就不记录
//         h--;
//         if (h == 0)break;//到最后一行退出
//     }
//
// }
//
// inline void img_process_object::left_get(uint16 total_L)
// {
//     uint8 i = 0;
//     uint16 j = 0;
//     uint8 h = 0;
//
//     //初始化
//     for (i = 0; i < RESULT_ROW; i++)
//     {
//         border_l[i] = 255;
//     }
//     h = start_h;
//
//     //左边
//     for (j = 0; j < total_L; j++)
//     {
//
//         if (points_l[j][1] == h)
//         {
//
//             border_l[h] = points_l[j][0] + 1;
//
//         }
//         else continue; //每行只取一个点，没到下一行就不记录
//
//         h--;
//         if (h == 0)
//         {
//             break;//到最后一行退出
//         }
//     }
//
// }
//
// inline void img_process_object::right_get(uint16 total_R)
// {
//     uint8 i = 0;
//     uint16 j = 0;
//     uint8 h = 0;
//     for (i = 0; i < RESULT_ROW; i++)
//     {
//         border_r[i] = 255;//右边线初始化放到最右边，左边线放到最左边，这样八邻域闭合区域外的中线就会在中间，不会干扰得到的数据
//     }
//     h = start_h;
//     //右边
//     for (j = 0; j < total_R; j++)
//     {
//         if (points_r[j][1] == h)
//         {
//             border_r[h] = points_r[j][0] - 1;
//
//         }
//         else continue;//每行只取一个点，没到下一行就不记录
//         h--;
//         if (h == 0)break;//到最后一行退出
//     }
//
// }
//
//
// inline uint8_t img_process_object::Threshold_deal(const cv::Mat& image, uint32_t pixel_threshold) {
//     // 检查图像是否为有效的单通道灰度图
//     if (image.empty() || image.type() != CV_8UC1) {
//         throw std::invalid_argument("Input image must be a non-empty grayscale image.");
//     }
//
//     const int GrayScale = 256; // 灰度级范围
//     int pixelCount[GrayScale] = {0}; // 灰度值统计
//     float pixelPro[GrayScale] = {0.0f}; // 灰度值比例
//     int width = image.cols; // 图像宽度
//     int height = image.rows; // 图像高度
//     int pixelSum = width * height; // 总像素数
//     uint8_t threshold = 0; // 最终阈值
//
//     // 统计灰度值分布
//     uint32_t gray_sum = 0; // 灰度值总和
//     for (int i = 0; i < height; i++) {
//         for (int j = 0; j < width; j++) {
//             uint8_t pixelValue = image.at<uint8_t>(i, j);
//             pixelCount[pixelValue]++;
//             gray_sum += pixelValue;
//         }
//     }
//
//     // 计算每个灰度值的比例
//     for (int i = 0; i < GrayScale; i++) {
//         pixelPro[i] = static_cast<float>(pixelCount[i]) / pixelSum;
//     }
//
//     // 遍历灰度级，寻找最佳阈值
//     float w0 = 0.0f, w1 = 0.0f;
//     float u0tmp = 0.0f, u1tmp = 0.0f;
//     float u0 = 0.0f, u1 = 0.0f, u = 0.0f;
//     float deltaTmp = 0.0f, deltaMax = 0.0f;
//
//     for (int j = 0; j < pixel_threshold; j++) {
//         w0 += pixelPro[j]; // 背景部分比例
//         u0tmp += j * pixelPro[j]; // 背景灰度值总和
//
//         w1 = 1.0f - w0; // 前景部分比例
//         u1tmp = static_cast<float>(gray_sum) / pixelSum - u0tmp; // 前景灰度值总和
//
//         if (w0 > 0.0f) u0 = u0tmp / w0; // 背景平均灰度
//         if (w1 > 0.0f) u1 = u1tmp / w1; // 前景平均灰度
//
//         u = u0tmp + u1tmp; // 全局平均灰度
//         deltaTmp = w0 * std::pow((u0 - u), 2) + w1 * std::pow((u1 - u), 2);
//
//         if (deltaTmp > deltaMax) {
//             deltaMax = deltaTmp;
//             threshold = j;
//         }
//     }
//
//     return threshold;
// }
//
// inline void img_process_object::Get01change(const cv::Mat& inputImage, cv::Mat& outputImage, int thresholdStatic) {
//     // 检查输入图像是否为单通道灰度图
//     if (inputImage.empty() || inputImage.type() != CV_8UC1) {
//         throw std::invalid_argument("Input image must be a non-empty grayscale image.");
//     }
//
//     // 初始化输出图像
//     outputImage = cv::Mat::zeros(inputImage.size(), CV_8UC1);
//
//     int rows = inputImage.rows;
//     int cols = inputImage.cols;
//
//     for (int i = 0; i < rows; i++) {
//         for (int j = 0; j < cols; j++) {
//             int threshold = thresholdStatic;
//
//             // 根据列位置调整阈值
//             if (j <= 15) {
//                 threshold -= 10;
//             } else if (j > 70 && j <= 75) {
//                 threshold -= 15;
//             } else if (j >= 65) {
//                 threshold -= 15;
//             }
//
//             // 获取当前像素值
//             uint8_t pixelValue = inputImage.at<uint8_t>(i, j);
//
//             // 应用阈值分割
//             if (pixelValue > threshold) {
//                 outputImage.at<uint8_t>(i, j) = 255;  // 白色
//             } else {
//                 outputImage.at<uint8_t>(i, j) = 0;    // 黑色
//             }
//         }
//     }
// }
//
// inline void img_process_object::Get01change_dajin(const cv::Mat& inputImage, cv::Mat& outputImage, int threshold_static, int threshold_detach) {
//     // 检查输入图像是否有效
//     if (inputImage.empty() || inputImage.channels() != 1) {
//         throw std::invalid_argument("Input image must be a non-empty grayscale image.");
//     }
//
//     // 初始化输出图像，与输入图像大小相同，类型为单通道二值图像
//     outputImage = cv::Mat::zeros(inputImage.size(), CV_8UC1);
//
//     // 计算动态阈值
//     int dynamicThreshold = Threshold_deal(inputImage, threshold_detach);
//     if (dynamicThreshold < threshold_static) {
//         dynamicThreshold = threshold_static;
//     }
//
//     // 遍历每个像素
//     for (int i = 0; i < inputImage.rows; i++) {
//         for (int j = 0; j < inputImage.cols; j++) {
//             int thre;
//
//             // 根据列位置动态调整阈值
//             if (j <= 15) {
//                 thre = dynamicThreshold - 10;
//             } else if (j > 70 && j <= 75) {
//                 thre = dynamicThreshold - 10;
//             } else if (j >= 65) {
//                 thre = dynamicThreshold - 10;
//             } else {
//                 thre = dynamicThreshold;
//             }
//
//             // 像素二值化
//             if (inputImage.at<uchar>(i, j) >= thre) {
//                 outputImage.at<uchar>(i, j) = 255; // 白
//             } else {
//                 outputImage.at<uchar>(i, j) = 0;   // 黑
//             }
//         }
//     }
// }
//
// inline void img_process_object::find_line()
// {
//     using namespace std::chrono; // 简化 chrono 的命名空间
//     cv::flip(mt9v03x_image, mt9v03x_image, -1);//摄像头反着装的，所有要反转一下
//     data_stastics_l = 0;
//     data_stastics_r = 0;
//     line_detect_center = use_h - 1;
//     //auto start_time = high_resolution_clock::now();
//     cv::Mat tempImage = mt9v03x_image.clone();
//
//     Get01change_dajin(tempImage, mt9v03x_image, 120, 220);
//
//     cv::Mat temp = mt9v03x_image.clone();
//
//     temp = inversePerspectiveTransform(temp, inverseMatrix, RESULT_ROW, RESULT_COL);//逆透视
//
//     image_draw_rectan(temp);
//     mt9v03x_image = temp.clone();
//
//     if (get_start_point(temp, (start_h))) {
//         search_l_r((uint16)USE_num, temp, start_point_l[0], start_point_l[1],
//                    start_point_r[0], start_point_r[1], &hightest);
//     }
//
//
//     vgetborder_maxwhitecols(temp);//最长白烈寻线
//     Element_Detection(temp);//元素检测
//     vget_center_shangjiao();//根据不同的丢线情况和元素，决定当前是使用左边线还是右边线作为中线
//     Error_offline();//误差计算
//
// }
// inline void img_process_object::vget_center_shangjiao() {
//     if (RightCirque_Flag==4) {//右补线
//         for (int i = RESULT_ROW - 1; i > line_detect_center; i--) {
//             center_line[i]=r_border[i];
//             if (center_line[i]>200) {
//                 center_line[i]=center_line[i+1];
//
//             }
//
//         }
//         center_line[RESULT_ROW]=1;//右边线
//         center_line_last[RESULT_ROW]=1;
//     }
//     else if (RightCirque_Flag==5) {//左边补右边
//         for (int i = RESULT_ROW - 1; i > line_detect_center; i--) {
//             center_line[i]=l_border[i];
//             if (center_line[i]>200) {
//                 center_line[i]=center_line[i+1];
//             }
//         }
//         center_line[RESULT_ROW]=0;//左边线
//         center_line_last[RESULT_ROW]=0;
//     }
//     else if (RightCirque_Flag==6) {//右补线
//         for (int i = RESULT_ROW - 1; i > line_detect_center; i--) {
//             center_line[i]=r_border[i];
//             if (center_line[i]>200) {
//                 center_line[i]=center_line[i+1];
//
//             }
//             center_line_last[i]=center_line[i];
//         }
//         center_line[RESULT_ROW]=1;//右边线
//
//     }
//     else if (RightCirque_Flag==7) {
//         for (int i = RESULT_ROW - 1; i > line_detect_center; i--) {
//             center_line[i]=l_border[i];
//             if (center_line[i]>200) {
//                 center_line[i]=center_line[i+1];
//
//             }
//
//             center_line_last[i]=center_line[i];
//         }
//         center_line[RESULT_ROW]=0;//左边线
//         center_line_last[RESULT_ROW]=0;
//     }
//     else if (LeftCirque_Flag==4) {//左补线
//         for (int i = RESULT_ROW - 1; i > line_detect_center; i--) {
//             center_line[i]=l_border[i];
//             if (center_line[i]>200) {
//                 center_line[i]=center_line[i+1];
//
//             }
//             center_line_last[i]=center_line[i];
//         }
//         center_line[RESULT_ROW]=0;//左线
//         center_line_last[RESULT_ROW]=0;
//     }
//     else if (LeftCirque_Flag==5) {//右边补左边
//         for (int i = RESULT_ROW - 1; i > line_detect_center; i--) {
//             center_line[i]=r_border[i];
//             if (center_line[i]>200) {
//                 center_line[i]=center_line[i+1];
//
//             }
//
//
//         }
//         center_line[RESULT_ROW]=1;//右边线
//
//     }
//     else if (LeftCirque_Flag==6) {//左补线
//         for (int i = RESULT_ROW - 1; i > line_detect_center; i--) {
//             center_line[i]=l_border[i];
//             if (center_line[i]>200) {
//                 center_line[i]=center_line[i+1];
//
//             }
//
//         }
//         center_line[RESULT_ROW]=0;//左边线
//         center_line_last[RESULT_ROW]=0;
//     }
//     else if (LeftCirque_Flag==7) {
//         for (int i = RESULT_ROW - 1; i > line_detect_center; i--) {
//             center_line[i]=r_border[i];
//             if (center_line[i]>200) {
//                 center_line[i]=center_line[i+1];
//
//             }
//
//
//         }
//         center_line[RESULT_ROW]=1;//右边线
//         center_line_last[RESULT_ROW]=1;
//     }
//     else if (R_Obstacle_flag) {//障碍物在右边
//         for (int i = RESULT_ROW - 1; i > line_detect_center; i--) {
//             center_line[i]=r_border[i];
//             if (center_line[i]>200) {
//                 center_line[i]=center_line[i+1];
//             }
//         }
//         center_line[RESULT_ROW]=1;
//     }
//     else if (L_Obstacle_flag) {//障碍物在左边 用右边的线
//         for (int i = RESULT_ROW - 1; i > line_detect_center; i--) {
//             center_line[i]=l_border[i];
//             if (center_line[i]>200) {
//                 center_line[i]=center_line[i+1];
//             }
//         }
//         center_line[RESULT_ROW]=0;
//     }
//     else{//非元素
//         if (l_border_lostnum<=r_border_lostnum) {//左补线
//             for (int i = RESULT_ROW - 1; i > line_detect_center; i--) {
//                 center_line[i]=l_border[i];
//                 if (center_line[i]>200) {
//                     center_line[i]=center_line[i+1];
//
//                 }
//                 // printf("center_line[i]=%d\n",center_line[i]);
//             }
//             center_line_last[RESULT_ROW]=0;
//             center_line[RESULT_ROW]=0;
//
//         }
//         else {//右补线
//             for (int i = RESULT_ROW - 1; i > line_detect_center; i--) {
//                 center_line[i]=r_border[i];
//                 if (center_line[i]>200) {
//                     center_line[i]=center_line[i+1];
//
//                 }
//
//             }
//             // printf("ok\n");
//             center_line_last[RESULT_ROW]=1;
//             center_line[RESULT_ROW]=1;
//         }
//
//     }
//
//
//
//
// }
//
//
//
//
// inline void img_process_object::draw_line_1(cv::Mat& image, const uint8_t border[], cv::Scalar color, int num) {
//     for(int i=0;i<num;i++){
//
//         cv::circle(image,cv::Point(border[i], i),1,color,-1);
//     }
// }
//
// inline void img_process_object::odemory_start(uint8 use_odemory) {
//     switch (use_odemory) {
//         case 0: if (odemory_flag0 == false) {
//                 last0_pulse = total_pulse;
//                 odemory_flag0 = true;
//             }
//             break;
//         case 1:
//             if (odemory_flag1 == false) {
//                 last1_pulse = total_pulse;
//                 odemory_flag1 = true;
//             }
//             break;
//         case 2:
//             if (odemory_flag2 == false) {
//                 last2_pulse = total_pulse;
//                 odemory_flag2 = true;
//             }
//             break;
//         case 3:
//             if (odemory_flag3 == false) {
//                 last3_pulse = total_pulse;
//                 odemory_flag3 = true;
//             }
//             break;
//         }
// }
//
// inline void img_process_object::odemory_get(uint8 use_odemory)
//             {
//             switch (use_odemory) {
//                     case 0:
//                         now0_pulse = total_pulse - last0_pulse;
//                         break;
//                     case 1:
//                         now1_pulse = total_pulse - last1_pulse;
//                         break;
//                     case 2:
//                         now2_pulse = total_pulse - last2_pulse;
//                         break;
//                     case 3:
//                         now3_pulse = total_pulse - last3_pulse;
//                         break;
//                 }
//
// }
//
// inline void img_process_object::odemory_stop(uint8 use_odemory) {
//             switch (use_odemory) {
//                 case 0:
//                     last0_pulse=0;
//                     now0_pulse=0;
//                     odemory_flag0=false;
//                     break;
//                 case 1:
//                     last1_pulse=0;
//                     now1_pulse=0;
//                     odemory_flag1=false;
//                     break;
//                 case 2:
//                     last2_pulse=0;
//                     now2_pulse=0;
//                     odemory_flag2=false;
//                     break;
//                 case 3:
//                     last3_pulse=0;
//                     now3_pulse=0;
//                     odemory_flag3=false;
//                     break;
//             }
//
// }
//
//
// inline void img_process_object::Element_Detection(const cv::Mat &inputImage) {
//     LINE_JUST(1);
//     LINE_JUST(2);//LEFT_just 1左连续 2右连续 3左不连续 4右不连续
//
//     zebra_crossing(inputImage);
//     // printf("Zebra_crossing_flag=%d\n",Zebra_crossing_flag);
//     if (Zebra_crossing_flag==0) {
//         circle_fill_right_qlh(inputImage);
//         circle_fill_left_qlh(inputImage);
//         cross_fill(inputImage);
//         // printf("Cross_flag=%d\n",Cross_Flag);
//         Staight_Detection(inputImage);
//         // printf("Straight_Flag=%d\n",Straight_Flag);
//         // podao(inputImage);
//         // printf("Slope_flag=%d\n",Slope_flag);
//         obstacle(inputImage);
//
//     }
//
// }
//
// inline void img_process_object::Staight_Detection(const cv::Mat &inputImage) {
//
//      bool error_flag = false;
//     static int straight_num = 0;
//
//
//     // 如果已经检测到圆环或十字，当前不可能是直道，直接退出
//     if (RightCirque_Flag >= 1 || LeftCirque_Flag >= 1 || Cross_Flag == 1) {
//         Straight_Flag = 0;
//         return;
//     }
//
//     // 创建两个点集，分别用于存储左边界和右边界上的点坐标
//     std::vector<cv::Point> left_points, right_points;
//
//     // 从图像下方向上遍历边界线像素点，构建拟合用的点集
//     // 注意：图像坐标中 (x, y)，x是列，y是行
//     for (int i = RESULT_ROW - 10; i > line_detect_center + 30; i--) {
//         // 左边界的点 (x = l_border[i], y = i)
//         left_points.emplace_back(l_border[i], i);
//         // 右边界的点 (x = r_border[i], y = i)
//         right_points.emplace_back(r_border[i], i);
//     }
//
//     // 定义一个 Lambda 函数，用于对点集进行直线拟合，并返回斜率
//     auto fitLineSlope = [](const std::vector<cv::Point>& pts) -> float {
//         // 如果点太少，返回一个极端斜率
//         if (pts.size() < 2) return 9999;
//
//         // 拟合直线，line 是 [vx, vy, x0, y0]，方向向量和通过点
//         cv::Vec4f line;
//         cv::fitLine(pts, line, cv::DIST_L2, 0, 0.01, 0.01);
//
//         // 返回直线的斜率 = vy / vx
//         return line[1] / line[0];
//     };
//
//     // 分别对左边界和右边界拟合直线，得到斜率
//     float k_left = fitLineSlope(left_points);
//     float k_right = fitLineSlope(right_points);
//
//     // 判断两条边是否“近似垂直”：
//     // 若斜率绝对值很大（例如 > 3），说明是接近垂直方向的直线
//     bool is_left_vertical = std::abs(k_left) > 3.0;
//     bool is_right_vertical = std::abs(k_right) > 3.0;
//
//     // 最终判断是否为“直线区域”的条件：
//     // 1. 当前检测中心点是有效的（等于offline）
//     // 2. 左右两边边界线都近似垂直
//     if (line_detect_center == offline && is_left_vertical && is_right_vertical) {
//         Straight_Flag = 1; // 是直道
//     }
//     else {
//         Straight_Flag = 0; // 不是直道
//
//     }
//
//     // if (straight_num>=3){
//     //     Straight_Flag = 1; // 是直道
//     //     straight_num=0;
//     // } else {
//     //     Straight_Flag = 0; // 不是直道
//     //     straight_num=0;
//     // }
//
//     // 打印调试信息：是否为直道，以及拟合出的两个斜率
//     // printf("Straight_Flag=%d, k_left=%.2f, k_right=%.2f\n", Straight_Flag, k_left, k_right);
// }
// static uint8 statics=0;
// //左右圆环 均不用找圆环的弧边点
//
//
// inline void img_process_object::circle_fill_right_qlh(const cv::Mat &inputImage) {
//     uint16 sum_r = 0,sum_l = 0;
//     static uint8 seed3=0;//v字点
//     int num_1=0,num_2=0;
//     static bool Right_Gap_Flag=false;
//
//     if(data_stastics_r>30&&RightCirque_Flag!=0)//右
//
//         {
//
//         sum_r=data_stastics_r;
//         sum_l=data_stastics_l;
//         if(data_stastics_r>USE_num)sum_r=180;//限幅
//     }
//
//     switch (RightCirque_Flag) {
//         case 0://left边连续right不连续认为是圆环
//             if (RIGHT_just==4&&LEFT_just==1&&l_border_lostnum==0&&r_border_lostnum>=25) {
//                 RightCirque_Flag=1;
//                 odemory_stop(0);
//             }
//             break;
//         case 1://历史遗留 没用
//             RightCirque_Flag=2;
//             break;
//         case 2://维持中间突变点
//             odemory_start(0);
//             odemory_get(0);
//             if (l_border_lostnum!=0) {
//                 RightCirque_Flag=0;
//                 odemory_stop(0);
//                 printf("l_border_lostnum have value\n");
//             }
//             if (now0_pulse>1.5) {
//                 odemory_stop(0);
//                 RightCirque_Flag=0;
//                 return;
//             }
//             for (int i=RESULT_ROW-13;i>12;i--) {
//                 if (r_border[i]<rlost_border&&r_border[i+12]<rlost_border
//                     &&r_border[i-12]<rlost_border&&r_border[i-6]<rlost_border
//                     &&r_border[i+6]<rlost_border&&r_border[i+12]>r_border[i+6]
//                     &&r_border[i+6]>=r_border[i]
//                     &&r_border[i-6]<=r_border[i-12]&&r_border[i]<r_border[i-6])
//                 {
//                     RightCirque_Flag = 3;
//                     Right_Gap_Flag = true;
//                     odemory_stop(0);
//                     break;
//                 }
//             }
//             // for(int i=RESULT_ROW-5;i>=line_detect_center;i--) {
//             //     // printf("r_border[%d]=%d\n",i,r_border[i]);
//             //
//             //     if (i > line_detect_center && i<85 &&(r_border[i] >= 95 && r_border[i] <= 115))
//             //     {
//             //         num_2++;
//             //     }
//             //     // printf("first:r_border_lostnum=%d,num_2=%d\n",r_border_lostnum,num_2);
//             //     if ((r_border_lostnum >= 30&&num_2>=15)&&now0_pulse>0.1){
//             //         RightCirque_Flag = 3;
//             //         // printf("okkkkkkkkkkkkkkkkkkk");
//             //         Right_Gap_Flag=true;
//             //         odemory_stop(0);
//             //         break;
//             //     }
//             // }
//
//             break;
//             case 3://寻找上V点
//             odemory_start(0);
//             odemory_get(0);
//             // if (l_border_lostnum>=10) {
//             //     RightCirque_Flag=0;
//             //     odemory_stop(0);
//             //     printf("l_border_lostnum have value\n");
//             //     return;
//             // }
//             if (now0_pulse>2) {
//                 RightCirque_Flag=0;
//                 odemory_stop(0);
//                 return;
//             }
//
//             for(int i=RESULT_ROW-5;i>=line_detect_center;i--) {
//                 // printf("r_border[%d]=%d\n",i,r_border[i]);
//
//                 if (i > line_detect_center && i<85 && r_border[i] >= 95 && r_border[i] <= 115 )
//                     {
//                         num_2++;
//                     }
//                 // printf("r_border_lostnum=%d,num_2=%d\n",r_border_lostnum,num_2);
//                 if ((r_border_lostnum>=30 && num_2 > 15)&&Right_Gap_Flag==1&&now0_pulse>=0.05) {
//                     RightCirque_Flag = 4;
//                     Right_Gap_Flag=false;
//                     odemory_stop(0);
//                     break;
//                 }
//
//             }
//             // printf("num_1=%d,num2=%d\n", num_1, num_2);
//
//             break;
//         case 4://判断是否进入圆环
//             odemory_start(0);
//             odemory_get(0);
//             LeftCirque_Flag=0;
//             if (l_border_lostnum<=1&&now0_pulse>0.5) {
//                 RightCirque_Flag=5;
//                 odemory_stop(0);
//             }
//             break;
//         case 5://等待两边同时出圆环
//             odemory_start(0);
//             odemory_get(0);
//             LeftCirque_Flag=0;
//             //开过了
//             if (LEFT_just==1) {
//                 RightCirque_Flag=0;
//                 odemory_stop(0);
//                 return;
//             }
//             if (l_border_lostnum>30&&now0_pulse>0.6) {
//                 RightCirque_Flag=6;
//                 odemory_stop(0);
//             }
//             //else printf("r_border_lostnum=%d,now_pulse=%f\n",r_border_lostnum,now_pulse);
//
//             break;
//         case 6://持续使用八邻域右边线,出圆环
//             odemory_start(0);
//             odemory_get(0);
//             LeftCirque_Flag=0;
//             if (LEFT_just==1) {
//                 RightCirque_Flag=0;
//                 odemory_stop(0);
//                 return;
//             }
//             if (l_border_lostnum<20&&inputImage.at<uint8>(30,135)==255&&now0_pulse>0.3) {
//                 RightCirque_Flag=7;
//                 odemory_stop(0);
//             }
//             break;
//         case 7:
//             odemory_start(0);
//             odemory_get(0);
//             LeftCirque_Flag=0;
//             if (now0_pulse>1) {
//                 RightCirque_Flag=0;
//                 odemory_stop(0);
//                 return;
//             }
//             /*稳定一段时间出圆环*/
//             break;
//         default:break;
//
//
//     }
//     switch (RightCirque_Flag) {
//         case 4:
//             get_right(data_stastics_r);
//             center_line[RESULT_ROW]=1;
//             break;
//         case 6:
//             get_right(data_stastics_r);
//             center_line[RESULT_ROW]=1;
//             break;
//         case 2:
//             center_line[RESULT_ROW]=0;
//             break;
//         default:
//             break;
//     }
//     // printf("RightCirqrue_Flag=%d\n",RightCirque_Flag);
// }
//
//
// inline void img_process_object::circle_fill_left_qlh(const cv::Mat &inputImage) {
//     uint16 sum_r = 0,sum_l = 0;
//     bool near_feature,far_feature;
//     uint8 near_num=0,far_num=0;
//     uint8 far_max=0,far_min=255;
//     static bool Left_Gap_Flag=false;
//     int num_1=0,num_2=0;
//     if(data_stastics_l>30&&LeftCirque_Flag!=0)//右
//     {
//
//         sum_r=data_stastics_r;
//         sum_l=data_stastics_l;
//         if(data_stastics_r>USE_num)sum_l=180;//限幅
//     }
//
//     switch (LeftCirque_Flag) {
//         case 0://右边连续左边不连续认为是圆环
//             if (RIGHT_just==2&&LEFT_just==3&&r_border_lostnum==0&&l_border_lostnum>=25) {
//                 LeftCirque_Flag=1;
//                 odemory_stop(1);
//             }
//             break;
//         case 1://是否检测到下v点
//             LeftCirque_Flag=2;
//             break;
//         case 2://维持中间突变点
//             odemory_start(1);
//             odemory_get(1);
//             if (r_border_lostnum!=0) {
//                 LeftCirque_Flag=0;
//                 printf("r_border_lostnum have value\n");
//                 odemory_stop(1);
//             }
//             if (now1_pulse>1.5) {
//                 odemory_stop(1);
//                 LeftCirque_Flag=0;
//                 return;
//             }
//             for (int i=RESULT_ROW-13;i>12;i--) {
//                 if (l_border[i]>llost_border&&l_border[i+12]>llost_border
//                     &&l_border[i-12]>llost_border&&l_border[i-6]>llost_border
//                     &&l_border[i+6]>llost_border&&l_border[i+12]<l_border[i+6]
//                     &&l_border[i+6]<=l_border[i]
//                     &&l_border[i]>l_border[i-6]&&l_border[i-6]>=l_border[i-12]
//                     )
//                     {
//                         LeftCirque_Flag = 3;
//                         Left_Gap_Flag = true;
//                         odemory_stop(1);
//                         break;
//                     }
//             }
//             break;
//         case 3://寻找上V点
//             odemory_start(1);
//             odemory_get(1);
//             // if (r_border_lostnum>=10) {
//             //     LeftCirque_Flag=0;
//             //     printf("r_border_lostnum have value\n");
//             //
//             //     odemory_stop(1);
//             //     return;
//             // }
//             if (now1_pulse>2) {
//                 LeftCirque_Flag=0;
//                 odemory_stop(1);
//                 return;
//             }
//
//             for(int i=RESULT_ROW-5;i>=line_detect_center;i--) {
//                 // printf("l_border[%d]=%d\n",i,l_border[i]);
//
//                 if (i > line_detect_center && i<85 && l_border[i] >= 50 && l_border[i] <= 65 )
//                 {
//                     num_2++;
//                 }
//                 // printf("l_border_lostnum=%d,num_2=%d\n",l_border_lostnum,num_2);
//                 if ((l_border_lostnum>=30 && num_2 >= 15)&&Left_Gap_Flag==1&&now1_pulse>=0.05) {
//                     LeftCirque_Flag = 4;
//                     // printf("l_border_lostnum=%d,num_2=%d\n",l_border_lostnum,num_2);
//                     Left_Gap_Flag=false;
//                     odemory_stop(1);
//                     break;
//                 }
//             }
//
//             break;
//         case 4://判断是否进入圆环
//             odemory_start(1);
//             odemory_get(1);
//             RightCirque_Flag=0;
//             if (r_border_lostnum<=1&&now1_pulse>0.5) {
//                 LeftCirque_Flag=5;
//                 odemory_stop(1);
//             }
//             break;
//         case 5://等待两边同时出圆环
//             odemory_start(1);
//             odemory_get(1);
//             RightCirque_Flag=0;
//             //开过了
//
//             if (RIGHT_just==2) {
//                 LeftCirque_Flag=0;
//                 odemory_stop(1);
//                 return;
//             }
//             if (r_border_lostnum>30&&now1_pulse>0.6) {
//                 LeftCirque_Flag=6;
//                 odemory_stop(1);
//             }
//
//             break;
//         case 6://持续使用八邻域右边线,出圆环
//             odemory_start(1);
//             odemory_get(1);
//             RightCirque_Flag=0;
//             if (RIGHT_just==2) {
//                 LeftCirque_Flag=0;
//                 odemory_stop(1);
//                 return;
//             }
//             if (r_border_lostnum<20&&inputImage.at<uint8>(25,25)==255&&now1_pulse>0.35) {
//                 LeftCirque_Flag=7;
//                 odemory_stop(1);
//             }
//             break;
//         case 7:
//             odemory_start(1);
//             odemory_get(1);
//             RightCirque_Flag=0;
//             if (now1_pulse>1) {
//                 LeftCirque_Flag=0;
//                 odemory_stop(1);
//                 return;
//             }
//             /*稳定一段时间出圆环*/
//             break;
//         default:break;
//
//
//     }
//     switch (LeftCirque_Flag) {
//         case 4:
//             get_left(data_stastics_l);
//             center_line[RESULT_ROW]=0;
//             break;
//         case 6:
//             get_left(data_stastics_l);
//             center_line[RESULT_ROW]=0;
//             break;
//         case 2:
//         case 3:
//             r_border_lostnum=0;
//             break;
//         default:
//             break;
//     }
//
//     // printf("LeftCirque_Flag=%d\n",LeftCirque_Flag);
// }
//
//
//
// inline void img_process_object::cross_fill(const cv::Mat &inputImage) {
//     float use_sum = 0;
//     uint8 break_num_l_up[2] = {0,0};
//     uint8 break_num_l_down[2] = {0,0};
//     uint8 break_num_r_up[2]= {0,0};
//     uint8 break_num_r_down[2]= {0,0};
//     bool off_left=false;
//     bool off_right=false;
//     uint8 start_r, end_r;
//     uint8 start_l, end_l;
//
//     // if (!(l_border_lostnum>30&&r_border_lostnum>30)) {
//     //     Cross_Flag = 0;
//     //     return;
//     // }
//     //用左右丢线数目来判别十字
//     for (int i = RESULT_ROW-1; i > line_detect_center; i--) {
//         if (l_border[i]==255&&r_border[i]==255) {
//             use_sum++;
//         }
//     }
//     // printf("use_num=%f\n",use_sum);
//     if (use_sum>10) {
//         Cross_Flag = 1;
//     }
//     else Cross_Flag = 0;
//     // printf("Cross_Flag:%d\n",Cross_Flag);
//     if (Cross_Flag==1) {
//         l_border_lostnum=0;
//         // 寻找左边界的下拐点 寻找右边界的下拐点
//         for (int i = (use_h - 1) / 2; i < RESULT_ROW - 1; i++) {
//             // 寻找左下拐点
//             if (l_border[i] >= 60 && l_border[i] != 255 && l_border[i - 1] == 255 && l_border[i + 1] != 255 && !
//                 off_left) {
//                 break_num_l_down[1] = i; // y
//                 break_num_l_down[0] = l_border[i]; // x
//                 // printf("左下拐点: i = %d, l_border[i] = %d\n", i, l_border[i]);
//                 off_left = true; // 一旦找到左下拐点，就停止寻找
//                 break; // 退出循环
//             } else {
//                 break_num_l_down[1] = 0; // y
//                 break_num_l_down[0] = 0; // x
//             }
//             // 寻找右下拐点
//             if (r_border[i] >= use_w / 2 && r_border[i] != 255 && r_border[i] <= 110 && r_border[i - 1] == 255 &&
//                 r_border[i + 1] != 255 && !off_right) {
//                 break_num_r_down[1] = i; // y
//                 break_num_r_down[0] = r_border[i]; // x
//                 // printf("右下拐点: i = %d, r_border[i] = %d\n", i, r_border[i]);
//                 off_right = true; // 一旦找到右下拐点，就停止寻找
//                 break; // 退出循环
//             } else {
//                 break_num_r_down[1] = 0; // y
//                 break_num_r_down[0] = 0; // x
//             }
//         }
//
//         // 寻找左边界的上拐点 寻找右边界的上拐点
//         for (int i = 70; i > 20; i--) {
//             // 寻找左上拐点
//             if (l_border[i] != 255 && l_border[i] <= 60 && l_border[i - 1] != 255 && l_border[i + 1] == 255 && !off_left) {
//                 break_num_l_up[1] = i; // y
//                 break_num_l_up[0] = l_border[i]; // x
//                 // printf("左上拐点: i = %d, l_border[i] = %d\n", i, l_border[i]);
//                 off_left = true; // 一旦找到左上拐点，就停止寻找
//                 break; // 退出循环
//             } else {
//                 break_num_l_up[1] = 0; // y
//                 break_num_l_up[0] = 0; // x
//             }
//             // 寻找右上拐点
//             if (r_border[i] >= use_w / 2 && r_border[i] != 255 && r_border[i] <= 110 && r_border[i - 1] != 255 &&
//                 r_border[i + 1] == 255 && !off_right) {
//                 break_num_r_up[1] = i; // y
//                 break_num_r_up[0] = r_border[i]; // x
//                 // printf("右上拐点: i = %d, r_border[i] = %d\n", i, r_border[i]);
//                 off_right = true; // 一旦找到右上拐点，就停止寻找
//                 break; // 退出循环
//             } else {
//                 break_num_r_up[1] = 0; // y
//                 break_num_r_up[0] = 0; // x
//             }
//         }
//
//         // 对左边界进行直线拟合
//         if (break_num_l_up[0] && break_num_l_down[0] == 0) {
//             start_l = break_num_l_up[1] - 3; // 根据找到的拐点位置确定拟合区间
//             for (int i = start_l; i < 99; i++) {
//                 if (break_num_l_up[0]<59) l_border[i]=59;
//                 else l_border[i] = break_num_l_up[0];
//             }
//             l_border_lostnum=0;
//             // center_line[RESULT_ROW]==0;
//         }
//
//         if (break_num_l_down[0] && break_num_l_up[0] == 0) {
//             end_l = break_num_l_up[1] + 3; // 根据找到的拐点位置确定拟合区间
//             for (int i = end_l; i > offline; i--) {
//                 if (break_num_l_up[0]<59) l_border[i]=59;
//                 else l_border[i] = break_num_l_down[0];
//             }
//             l_border_lostnum=0;
//             // center_line[RESULT_ROW]==0;
//         }
//
//         //对右边界进行直线拟合
//         // if (break_num_r_up[0] && break_num_r_down[0] == 0) {
//         //     start_r = break_num_r_up[1] - 3; // 根据找到的拐点位置确定拟合区间
//         //     for (int i = start_l; i < 99; i++) {
//         //         r_border[i] = break_num_r_up[0];
//         //     }
//         //     r_border_lostnum=0;
//         // }
//         //
//         // if (break_num_r_down[0] && break_num_r_up[0] == 0) {
//         //     end_r = break_num_r_up[1] + 3; // 根据找到的拐点位置确定拟合区间
//         //     for (int i = end_l; i > offline; i--) {
//         //         r_border[i] = break_num_r_down[0];
//         //     }
//         //     r_border_lostnum=0;
//         // }
//     }
//
//
// }
//
//
//
//
//
// inline void img_process_object::CircleAddingLine(uint8 *l_border, uint8 *r_border,uint8_t status, uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY)
// {
//     int i = 0;
//
//     // 直线 x = ky + b
//     float k = 0.0f, b = 0.0f;
//     switch(status)
//     {
//         case 1://左补线
//         {
//             k = ((float)endX - (float)startX) / ((float)endY - (float)startY);
//             b = (float)startX - (float)startY * k;
//
//             for(i = startY; i < endY; i++)
//             {
//                 l_border[i]= (uint8_t)(k * i + b);
//             }
//             break;
//         }
//
//         case 2://右补线
//         {
//             k = ((float)endX - (float)startX) / ((float)endY - (float)startY);
//             b = (float)startX - (float)startY * k;
//
//             for(i = startY; i < endY; i++)
//             {
//                 r_border[i] = (uint8_t)(k * i + b);
//             }
//             break;
//         }
//
//     }
// }
//
// inline int img_process_object::My_Abs(int a, int b)
// {
//
//     if ((a - b) > 0)
//         return ((int)(a - b));
//     else return ((int)(b - a));
// }
//
// inline void img_process_object:: fingdoubleright()
// {
//     uint8 i = 0;
//     uint8 rightduan1 = 0;
//     uint8 rightduan2 = 0;
//     uint8 newopen = 0;
//     truedoubleright = 0;
//
//     for (i = 4; i <= use_h; i++)
//     {
//         if (My_Abs(r_border[i], r_border[i + 1]) > 5)
//         {
//             rightduan1 = i + 1;
//             break;
//         }
//         if (i >= 12 && r_border[i] == 0 && r_border[i + 1] != 0)
//         {
//             rightduan1 = i + 1;
//             break;
//         }
//         if (i == 28) break;
//     }
//     if (rightduan1 != 0)
//     {
//         for (i = rightduan1 + 1; i <= use_h; i++)
//         {
//             if (My_Abs(r_border[i], r_border[i + 1]) < 4 && My_Abs(r_border[i + 1], r_border[i + 2]) < 4)
//             {
//                 newopen = i;
//                 break;
//             }
//         }
//     }
//     if (newopen != 0)
//     {
//         for (i = newopen; i <= use_h; i++)
//         {
//             if (My_Abs(r_border[i], r_border[i + 1]) > 5 && (i + 1) - rightduan1 >= 5)
//             {
//                 rightduan2 = i + 1;
//                 break;
//             }
//         }
//     }
//     if (rightduan2 != 0 && rightduan2 >= rightduan1 + 5)
//     {
//         for (i = rightduan2 + 1; i <= use_h; i++)
//         {
//             if (My_Abs(r_border[i], r_border[i + 1]) < 4 && r_border[i] != 0)
//             {
//                 truedoubleright = 1;
//                 break;
//             }
//         }
//     }
// }
//
// inline void img_process_object::fingdoubleleft()
// {
//     uint8 i = 0;
//     uint8 leftduan1 = 0;
//     uint8 leftduan2 = 0;
//     uint8 newopen = 0;
//     truedoubleleft = 0;
//
//     for (i = 4; i <= use_h; i++)
//     {
//         if (My_Abs(l_border[i], l_border[i + 1]) > 5)
//         {
//             leftduan1 = i + 1;
//             break;
//         }
//         if (i >= 12 && l_border[i] == 0 && l_border[i + 1] != 0)
//         {
//             leftduan1 = i + 1;
//             break;
//         }
//         if (i == 28) break;
//     }
//     if (leftduan1 != 0)
//     {
//         for (i = leftduan1 + 1; i <= use_h; i++)
//         {
//             if (My_Abs(l_border[i], l_border[i + 1]) < 4 && My_Abs(l_border[i + 1], l_border[i + 2]) < 4)
//             {
//                 newopen = i;
//                 break;
//             }
//         }
//     }
//     if (newopen != 0)
//     {
//         for (i = newopen; i <= use_h; i++)
//         {
//             if (My_Abs(l_border[i], l_border[i + 1]) > 5 && (i + 1) - leftduan1 >= 5)
//             {
//                 leftduan2 = i + 1;
//                 break;
//             }
//         }
//     }
//     if (leftduan2 != 0 && leftduan2 >= leftduan1 + 5)
//     {
//         for (i = leftduan2 + 1; i <= use_h; i++)
//         {
//             if (My_Abs(l_border[i], l_border[i + 1]) < 4 && l_border[i] != 0)
//             {
//                 truedoubleleft = 1;
//                 break;
//             }
//         }
//     }
// }
//
// inline uint8 img_process_object::get_prospect() {
//     uint8 prospect;
//     if (Straight_Flag==1) prospect=60;
//     else prospect=50;
//     if (Cross_Flag==1) {
//         if (line_detect_center<30) {
//             for (int i=70;i<RESULT_ROW;i++) {
//                 if (center_line[i]<104.5&&center_line[i]>60.5&&i<85) {
//                     prospect=i;
//                     break;
//                 }
//                 else if(i>85) prospect=40;
//             }
//         }
//     }
//
//     if (RightCirque_Flag==4) {
//         prospect=67;
//     }
//     else if (RightCirque_Flag==5)
//         prospect=65;
//     else if (RightCirque_Flag==6) {
//         prospect=70;
//     }
//     if (LeftCirque_Flag==4) {
//         prospect=65;
//     }
//     else if (LeftCirque_Flag==5)
//         prospect=60;
//     else if (LeftCirque_Flag==6) {
//         prospect=70;
//     }
//
//     if (Slope_flag==1) {
//         prospect=85;
//         LeftCirque_Flag=0;
//         RightCirque_Flag=0;
//     }
//
//     // if (Zebra_crossing_flag==1) {
//     //     for (int i = 70; i < RESULT_ROW; i++) {
//     //         if (center_line[i] < 85 && center_line[i] > 70 && i < 85) {
//     //             prospect = 30;
//     //             break;
//     //         } else if (i > 85) prospect = 85;
//     //     }
//     // }
//
//     if (prospect<line_detect_center) prospect=line_detect_center;
//     // printf("prospect=%d\n", prospect);
//     return prospect;
//
// }
//
//
// uint8 Weight2[50] = {//4/5
//
//     //10, 10, 10, 10,10, 10,10, 10, 10,10,
//     //10, 10, 11, 26 , 27, 31, 30, 25, 25, 10,
//     10, 15, 15, 40, 40, 40, 40 , 40, 40, 40, 15, 15, 10,
//     // 0, 0, 0, 10, 10, 11, 26 , 27, 31, 30, 25, 25, 10,
//     7, 6, 6, 6, 5, 4, 4, 4, 3, 3,
//     1, 1,  1,  1,  1,  1,  1,  1,  1,  1,
//     8,  8,  6,  4,  2,  2,  2,  2,  1,  1,
//     1,  1,  1,  1,  1,
//     //1,  1,  1,  1,  1,
// };
//
// float last_Det_True=0;
//
//
//
// inline void img_process_object::Error_offline()
// {
//     static float RightCirque_MAX_Det=0;
//     float sum=0.0f;
//     float weight_count=0.0f;
//     uint8 y1;
//     uint8 prospect;
//
//     prospect=get_prospect();
//     float mid_line=0;
//     if (center_line[RESULT_ROW]==0) {
//         // if (L_Obstacle_flag==true) mid_line=70;
//         // else
//         mid_line=59;
//     }
//     else if (center_line[RESULT_ROW]==1) {
//        mid_line=103;
//     }
//
//     points_test[0][1]=prospect;
//     points_test[0][0]=center_line[prospect];
//     // printf("points_test[0][0]=%d,points_test[0][1]=%d\n",points_test[0][0],points_test[0][1]);
//     for(uint8 j=prospect;j<=prospect+13;j++)
//     {
//         y1=j-prospect;
//         if (j>=RESULT_ROW) center_line[j]=center_line[j-1];
//         if (center_line[j]==0) center_line[j]=center_line_last[j];
//         center_line_last[j]=center_line[j];
//         sum+=center_line[j]*Weight2[y1];
//     }
//     for(uint8 x=0;x<13;x++)
//     {
//         weight_count+=Weight2[x];
//     }
//
//
//     Det_True=sum/weight_count-mid_line;//加权点
//     Det_True=-(Det_True*0.8+last_Det_True*0.2);
//     // if (Slope_flag==1) {
//     //     if (abs(Det_True)>10) {
//     //         Det_True=last_Det_True;
//     //     }
//     // }
//     if (Zebra_crossing_flag==1) Det_True=0;
//     // if (Det_True>-3&&Det_True<3) Det_True=0;
//     last_Det_True=Det_True;
//     //if(Det_True>5)
//     // printf("Det_True=%f\n",Det_True);
//
//     static float DetSlopeQueue[3] = {0};  // 储存最近3次斜率
//     static int slope_index = 0;
//
//     DetQueue1[det_index1] = Det_True;
//     int prev_index1 = (det_index1 - DET_DIFF_INTERVAL1 + DET_QUEUE_SIZE1) % DET_QUEUE_SIZE1;
//     float det_slope1 = (fabs(DetQueue1[det_index1]) - fabs(DetQueue1[prev_index1])) / DET_DIFF_INTERVAL1;
//     det_index1 = (det_index1 + 1) % DET_QUEUE_SIZE1;
//
//     DetQueue[det_index] = line_detect_center;
//     int prev_index = (det_index - DET_DIFF_INTERVAL + DET_QUEUE_SIZE) % DET_QUEUE_SIZE;
//     float det_slope = (fabs(DetQueue[det_index]) - fabs(DetQueue[prev_index])) / DET_DIFF_INTERVAL;
//     det_index = (det_index + 1) % DET_QUEUE_SIZE;
//
//     DetSlopeQueue[slope_index] = det_slope;
//     slope_index = (slope_index + 1) % 3;
//
//     bool slope_up = false;
//     bool slope_down = false;
//     bool slope_flat = false;
//     for (int i = 0; i < 2; i++) {
//         if (DetSlopeQueue[i] > 1.5f) slope_up = true;
//         if (DetSlopeQueue[i] < -1.0f) slope_down = true;
//         if (fabs(DetSlopeQueue[i]) <= 1.5f) slope_flat = true;
//     }
//     // printf("Detslope1:%f\n",DetSlopeQueue[0]);
//     // printf("Detslope2:%f\n",DetSlopeQueue[1]);
//     // printf("Detslope3:%f\n",DetSlopeQueue[2]);
//
//
//     if (slope_up && line_detect_center>40) {
//         bend_flag = 1;
//
//     } else if (slope_down&&line_detect_center<45) {
//         bend_flag = 3;
//     }else bend_flag = 0;
//
//     // printf("bend_flag = %d\r\n",bend_flag);
//
//     weight_count=0;
//     sum=0;
// }
//
// inline void img_process_object::LINE_JUST(uint8 staus)
// {
//     uint8 unm;
//     unm=0;
//     switch(staus)
//     {
//         case 1://左线单调判断 1连续 3不连续
//         {
//             for(uint8 i=95;i>(30);i--)
//             {
//                 if (l_border[i]<=65&&l_border[i]>=55&&l_border_lostnum<2)
//                 {
//                     unm++;
//                 }
//                 else if(l_border_lostnum>=2)
//                 {
//                     unm=0;
//                     LEFT_just=3;
//
//                     break;
//                 }
//             }
//             if(unm>=(RESULT_ROW)/2)//大于多少阈值时成立
//             {
//                 LEFT_just=1;
//             }else LEFT_just=3;
//         }break;
//         case 2://右线单调判定 2连续 4不连续
//         {
//             for(uint8 i=95;i>(30);i--)
//             {
//                 if (r_border[i]<=110&&r_border[i]>=100&&r_border_lostnum<2)
//                 {
//                     unm++;
//                 }
//                 else if(r_border_lostnum>=2)
//                 {
//                     unm=0;
//                     RIGHT_just=4;
//                     break;
//                 }
//             }
//             if(unm>=(RESULT_ROW)/2)
//             {
//                 RIGHT_just= 2;
//             }
//             else RIGHT_just= 4;
//         }break;
//     }
// }
//
//
// inline void img_process_object::zebra_crossing(const cv::Mat &inputImage)
// {
//     uint8 region = 0;
//     const uint8 white_thresh = 20;
//     const uint8 change_thresh = 10;
//     const uint8 min_region_rows = 5;
//     // Zebra_crossing_flag = false;
//     odemory_start(3);
//     odemory_get(3);
//     for (uint8 hang = line_detect_center; hang < RESULT_ROW-20; hang++) // 图像底部
//     {
//         uint8 row_white_count = 0;
//         uint8 transitions = 0;
//         uint8 prev_pixel = inputImage.at<uchar>(hang, 60); // 起始列靠中间
//
//         for (uint8 lie =60; lie < 100; lie++)  // 中间区域，避免边缘干扰
//         {
//             uint8 cur_pixel = inputImage.at<uchar>(hang, lie);
//             if (cur_pixel != 0) row_white_count++;
//
//             if ((cur_pixel != 0) != (prev_pixel != 0))
//                 transitions++;
//
//             prev_pixel = cur_pixel;
//         }
//         // printf("row_white_count=%d\n", row_white_count);
//         // printf("transitions=%d\n", transitions);
//
//         if (row_white_count > white_thresh && transitions > change_thresh)
//
//             region++;
//     }
//     // printf("region=%d\n", region);
//
//     if (region >= min_region_rows)
//     {
//         Zebra_crossing_flag = true;
//     }
//
//     if (now3_pulse>=0.5 && Zebra_crossing_flag == 1)
//     {
//         Zebra_crossing_flag = false;
//         odemory_stop(3);
//     }
//
//     // printf("Zebra_crossing_flag=%d\n",Zebra_crossing_flag);
// }
//
// inline int img_process_object::Lose_line_L(uint8 *l_border)
// {
//     uint8 i=0;
//     uint8 Just_L=0;
//
//     for(i=60;i>(hightest+5);i--)//从下往上
//     {
//         if(l_border[i]<=8)
//         {
//             Just_L++;
//         }
//     }
//     if(Just_L>25) return Just_L;
//
//     else return 0;
// }
//
// inline int img_process_object::Lose_line_R(uint8 *r_border)
// {
//    uint8 i=0;
//    uint8 Just_R=0;
//
//     for(i=60;i>(hightest+5);i--)//从下往上
//     {
//         if(r_border[i]>=92)
//         {
//             Just_R++;
//         }
//     }
//     if(Just_R>25) return Just_R;
//     else return 0;
// }
//
// inline void img_process_object::podao(const cv::Mat &inputImage) {
//     uint16 data=0;
//     data=max(data_stastics_l,data_stastics_r);
//     int track_wide[300];
//     uint8 num=0;
//     uint8 l_border_num=0;
//     uint8 r_border_num=0;
//     uint8 r_l_num=0;
//     odemory_get(3);
//
//     if (now3_pulse>=2.0)
//     {
//         Slope_flag=false;
//         odemory_stop(3);
//     }
//
//     if (RightCirque_Flag>=5||LeftCirque_Flag>=5||Cross_Flag==1) {
//         Slope_flag=false;
//         return;
//     }
//
//     right_get(data_stastics_r);
//     left_get(data_stastics_l);
//     for (uint8 i=RESULT_ROW-30; i >= hightest+1; i--)
//     {
//         track_wide[i]=border_r[i]-border_l[i];
//         // printf("track_wide[%d]=%d\n",i,track_wide[i]);
//
//         //弯道与坡道的区别
//         if (abs(border_l[i]-border_l[i-1])>=3||abs(border_r[i]-border_r[i-1])>=3) {
//             r_l_num++;
//         }
//         // printf("l_border[i]-l_border[i-1]=%d\n",l_border[i]-l_border[i-1]);
//         // printf("r_border[i]-r_border[i-1]=%d\n",r_border[i]-r_border[i-1]);
//         // printf("r_l_num=%d\n",r_l_num);
//         if (r_l_num)
//         {
//             Slope_flag=false;
//             r_l_num=0;
//             break;
//         }
//
//         //防止在左右圆环上 防止误判
//         if (border_l[i]<=border_min+5)
//         {
//             l_border_num++;
//         }
//
//         if (border_r[i]>=border_max-5)
//         {
//             r_border_num++;
//         }
//
//         if (l_border_num>=15||r_border_num>=15)
//         {
//             Slope_flag=false;
//             l_border_num=0;
//             r_border_num=0;
//             break;
//         }
//         //判断坡道
//         if (track_wide[i]>=55&&track_wide[i]<=100)
//         {
//             num++;
//         }
//         // printf("num=%d\n",num);
//         if (num>=40)
//         {
//             Slope_flag=true;
//             odemory_start(3);
//             num=0;
//             break;
//         }
//     }
//     // printf("Slope_flag=%d\n",Slope_flag);
//     if (Slope_flag==1&&now3_pulse>=1.5)
//     {
//         odemory_stop(3);
//         Slope_flag=false;
//     }
// }
//
// uint8 straight_track_wide[60] = {
//     0,0,0,0,0,0,0,0,0,0,//0-9
//     0,0,0,0,0,0,0,0,0,0,//10-19
//     24,27,31,33,36,39,42,44,47,50,//20-29
//     54,56,59,62,65,67,70,73,75,78,//30-39
//     81,83,86,88,89,90,91,93,94,94,//40-49
//     94,94,94,94,94,94,94,94,94,92
// };
//
// inline void img_process_object:: obstacle(const cv::Mat &inputImage) {
//     uint8 i=0;
//     uint8 r_num=0;
//     uint8 l_num=0;
//     uint8 track_wide[use_h];
//     R_Obstacle_flag=false;
//     L_Obstacle_flag=false;
//
//     if (LeftCirque_Flag>=1||RightCirque_Flag>=1||Zebra_crossing_flag==1||Cross_Flag==1||Slope_flag==1) {
//         R_Obstacle_flag=false;
//         L_Obstacle_flag=false;
//         // printf("erro");
//         return;
//
//     }
//     for (i=RESULT_ROW-1;i>line_detect_center+10;i--) {
//         track_wide[i] = r_border[i]-l_border[i];
//         // printf("track_wide[i]=%d\n",track_wide[i]);
//         if (track_wide[i]<=35&&track_wide[i]>=15) {
//             if (l_border[i]>65) l_num++;
//             if (r_border[i]<100) r_num++;
//         }
//
//     }
//     if (r_num>30) {//如果障碍物放在赛道的右边 用左边来巡线
//         R_Obstacle_flag=true;
//         L_Obstacle_flag=false;
//         printf("obstcle is right\n");
//     }
//     else if (l_num>30) {//如果障碍物放在赛道的左边 用右边来巡线
//         L_Obstacle_flag=true;
//         R_Obstacle_flag=false;
//         printf("obstcle is left\n");
//     }
//     else {
//         R_Obstacle_flag=false;
//         L_Obstacle_flag=false;
//     }
//
// }
//
// inline void img_process_object:: cube_recognize(const cv::Mat &inputImage) {
//
//     cv::Mat hsv, mask1, mask2, mask;
//     // 将原始 BGR 图像转换为 HSV 色彩空间，更适合颜色识别
//     // printf("inputImage channels: %d\n", inputImage.channels());
//
//     cv::cvtColor(inputImage, hsv, cv::COLOR_BGR2HSV);
//
//     // 提取红色区域（HSV 中红色跨越两个区域：低 H 和高 H）
//     cv::inRange(hsv, cv::Scalar(0, 70, 50), cv::Scalar(10, 255, 255), mask1);
//     cv::inRange(hsv, cv::Scalar(160, 70, 50), cv::Scalar(180, 255, 255), mask2);
//     cv::bitwise_or(mask1, mask2, mask);
//     // printf("Red pixels count: %d\n", cv::countNonZero(mask));
//
//
//     // 形态学处理：闭运算（先膨胀再腐蚀）去除小黑洞，填补小白点
//     cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1, -1), 2);
//
//     // 查找掩码中的轮廓（只找外轮廓）
//     std::vector<std::vector<cv::Point>> contours;
//     cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//
//     Cube_flag=false;
//     if (!contours.empty()) {
//         Cube_flag=true;
//     }else {
//         printf("contours is empty");
//         Cube_flag=false;
//     }
//
//     printf("Cube_flag=%d\n", Cube_flag);
//     // // 遍历所有轮廓
//     // for (const auto& contour : contours) {
//     //     printf("okkkkkkkkkkkkkkkkkkkkkkkkkkkkkk");
//     //     double area = cv::contourArea(contour);  // 计算轮廓面积
//     //     if (area < 300) continue;               // 忽略面积太小的干扰项
//     //
//     //     // 获取轮廓的外接矩形
//     //     cv::Rect rect = cv::boundingRect(contour);
//     //     float aspect = (float)rect.width / rect.height;  // 宽高比
//     //     printf("area=%lf\n", area);
//     //     printf("aspect=%f\n", aspect);
//     //     // 判断是否近似为正方形（宽高比接近 1）
//     //     if (aspect > 0.7 && aspect < 1.3) {
//     //         // 绘制绿色矩形框选区域
//     //         // cv::rectangle(inputImage, rect, cv::Scalar(0, 255, 0), 2);
//     //         // // 标注文字 "Cube"
//     //         // cv::putText(inputImage, "Cube", rect.tl(), cv::FONT_HERSHEY_SIMPLEX, 0.6,
//     //         //             cv::Scalar(255, 0, 0), 2);
//     //         Cube_flag=true;
//     //     }
//     //     else {
//     //         Cube_flag=false;
//     //     }
//     // }
//     // printf("Cube_flag=%d\n",Cube_flag);
// }
//
// #endif //IMG_PROCESS_H