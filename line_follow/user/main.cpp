// ============================================
// 智能车视觉自主导航主控程序
// 平台：龙芯2K0300 (LoongArch) + Buildroot
// ============================================

// ---------- 视觉处理模块 ----------
#include "VisionPipeline.h"
#include "ImgPreprocess.h"
#include "ImgLineTracker.h"
#include "ImgElementDetector.h"
#include "ImgOdometer.h"
#include "ImgCenterLine.h"
#include "ImgYolo.h"

// ---------- 控制与算法 ----------
#include "pid_.h"
#include "imu.h"
#include "key.hpp"
#include "Menu.h"
#include "GPIO.h"
#include "params.h"

// ---------- 龙芯逐飞库 ----------
#include "zf_device_imu.hpp"
#include "zf_driver_encoder.hpp"
#include "zf_driver_gpio.hpp"
#include "zf_driver_pwm.hpp"
#include "zf_driver_delay.hpp"
#include "zf_device_ips200_fb.hpp"
#include "zf_driver_pit.hpp"
#include "zf_device_dl1x.hpp"

// ---------- 标准库 ----------
#include <fstream>
#include <regex>
#include <math.h>

// ============================================
// region 宏定义
// ============================================
#define CAM_WIDTH     160
#define CAM_HEIGHT    120
#define CAM_FPS       180
#define CAM_PATH      "/dev/video0"
#define CAM_AUTO_EXPOSURE_DISABLE 1
#define CAM_DEFAULT_EXPOSURE      300

#define SWITCH_1     ZF_GPIO_KEY_1
#define BEEP_PATH    ZF_GPIO_BEEP
#define ENCODER_QUAD_1_PATH  ZF_ENCODER_QUAD_1
#define ENCODER_QUAD_2_PATH  ZF_ENCODER_QUAD_2
#define PWM_1_PATH   ZF_PWM_MOTOR_1
#define DIR_1_PATH   ZF_GPIO_MOTOR_1
#define PWM_2_PATH   ZF_PWM_MOTOR_2
#define DIR_2_PATH   ZF_GPIO_MOTOR_2
#define ESC_PATH     ZF_PWM_ESC_1
#define MAX_DUTY     (9500)
#define BATTERY_DIV_RATIO  (11)

// ============================================
// region 全局变量（必须在函数定义之前声明）
// ============================================
using namespace std;
using namespace cv;

// ----- 图像与摄像头（必须在cleanup之前声明） -----
cv::VideoCapture cap;              // 摄像头对象
Mat img_lhc, frame, gray, img_new;
volatile double error0;

// ----- 电机控制 -----
int16 duty_left, duty_right;
float overspeed;
float duty_left_press, duty_right_press;
float v_left, v_right;
float encoder_left[5] = {0};
float encoder_right[5] = {0};
float v_target, v_target_left, v_target_right;
int16 corr_l, corr_r;
volatile float corr_position;
float corr_gyroz;

// ----- 程序状态 -----
volatile bool runFlag = false;
volatile bool runFlagOld = false;
volatile bool program_exiting = true;
uint8_t Key, Mode = 1, Mode_Old = 0, last_switch0;

// ----- 编码器菜单 -----
double pulse, last_total_pulse;
const double encoder_step_threshold = 20.0f;

// ----- PID参数 -----
float p_l, i_l, d_l, p_r, i_r, d_r;
float p_position, d_position, p_gyroz, d_gyroz;

// ----- 电源监测（预留） -----
uint16 adc_reg = 0;
float adc_scale = 0;
uint16 battery_vol = 0;
uint16_t v_flag;

// ----- 里程计与IMU -----
constexpr float k = 0.0006;
constexpr float imu_k = 50;
int16 imu_gyro_z;
double zebra_meter = 0, gray_meter = 0;

// ----- PID对象指针 -----
IncrementalPID* pid_left_ctrl = nullptr;
IncrementalPID* pid_right_ctrl = nullptr;
PositionPID*    pid_position = nullptr;
GyrozPID*       pid_gyroz = nullptr;

// ----- 图像回调标志 -----
volatile bool g_image_ready = false;
volatile bool g_program_exit = false;

// ----- 元素状态记忆 -----
static int last_cross_flag = 0;
static int last_left_cirque_flag = 0;
static int last_right_cirque_flag = 0;
static int last_l_obstacle_flag = 0;
static int last_r_obstacle_flag = 0;
static int last_slope_flag = 0;
static int beep_countdown = 0;

// ----- 绕行策略 -----
uint8_t strategy_state = 0;
int turn_dir = 0;
enum dl1x_device_type_enum dl1x_dev_type = NO_FIND_DEVICE;

// ----- PWM设备信息 -----
struct pwm_info drv8701e_pwm_1_info;
struct pwm_info drv8701e_pwm_2_info;
struct pwm_info esc_info;

// ----- 图像拷贝缓冲区（TCP图传预留） -----
uint16 image_copy[RESULT_ROWS][RESULT_COLS];

// ============================================
// region 视觉/硬件对象实例化
// ============================================
ImgPreprocess preprocess;
ImgLineTracker lineTracker;
ImgOdometer odometer;
ImgElementDetector elementDetector(lineTracker, odometer);
ImgCenterLine centerLine(lineTracker, elementDetector);
ImgYolo yolo(lineTracker, elementDetector, odometer, centerLine);

zf_device_dl1x dl1x_dev;
zf_driver_gpio ncnn_1("/dev/zf_gpio_ncnn_1", O_RDONLY);
zf_driver_gpio ncnn_2("/dev/zf_gpio_ncnn_2", O_RDONLY);
zf_driver_gpio ncnn_3("/dev/zf_gpio_ncnn_3", O_RDONLY);
zf_driver_encoder encoder_quad_1(ENCODER_QUAD_1_PATH);
zf_driver_encoder encoder_quad_2(ENCODER_QUAD_2_PATH);
zf_driver_gpio  beep(BEEP_PATH, O_RDWR);
zf_driver_gpio  gpio_switch(SWITCH_1, O_RDWR);
zf_device_imu   imu_dev;
zf_driver_gpio  drv8701e_dir_1(DIR_1_PATH, O_RDWR);
zf_driver_gpio  drv8701e_dir_2(DIR_2_PATH, O_RDWR);
zf_driver_pwm   drv8701e_pwm_1(PWM_1_PATH);
zf_driver_pwm   drv8701e_pwm_2(PWM_2_PATH);
zf_driver_pwm   esc_pwm(ESC_PATH);
zf_device_ips200 ips;
zf_driver_pit   pit_control;

auto Menu = new menuNavigator(ips);

// ============================================
// region 工具函数
// ============================================

/** 读取NCNN硬件状态（3位GPIO） */
inline uint8_t read_ncnn_state()
{
    uint8_t b0 = ncnn_1.get_level();
    uint8_t b1 = ncnn_2.get_level();
    uint8_t b2 = ncnn_3.get_level();
    return (b2 << 2) | (b1 << 1) | b0;
}

/** 编码器读取与里程计更新 */
void encoder_get()
{
    v_left  = static_cast<float>(encoder_quad_1.get_count()) * 1.25f;
    v_right = static_cast<float>(-encoder_quad_2.get_count()) * 1.25f;
    encoder_quad_1.clear_count();
    encoder_quad_2.clear_count();
    odometer.total_pulse += ((fabs(v_left) + fabs(v_right)) * 0.5f * 0.05f * k);
}

/** 模板限幅函数 */
template <typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

/** 左轮输出（DIR物理反接修正：set_level与右轮相反） */
inline void motor_left_output(int16_t duty)
{
    if (duty >= 0) {
        drv8701e_dir_2.set_level(0);   // 反接修正：0=正转
        drv8701e_pwm_2.set_duty(duty);
    } else {
        drv8701e_dir_2.set_level(1);   // 反接修正：1=反转
        drv8701e_pwm_2.set_duty(-duty);
    }
}

/** 右轮输出（接线正常） */
inline void motor_right_output(int16_t duty)
{
    if (duty >= 0) {
        drv8701e_dir_1.set_level(1);
        drv8701e_pwm_1.set_duty(duty);
    } else {
        drv8701e_dir_1.set_level(0);
        drv8701e_pwm_1.set_duty(-duty);
    }
}

// ============================================
// region PIT回调（3ms周期，控制核心）
// ============================================

void pit_callback()
{
    static uint32_t tick = 0;
    tick++;

    // ---- Mode 1: 菜单调参（编码器作为菜单输入）----
    if (Mode == 1)
    {
        v_right = -encoder_quad_2.get_count();
        pulse += v_right * 0.05f;
        if (pulse - last_total_pulse > encoder_step_threshold)
        {
            encoderUpdateUPFlag = true;
            last_total_pulse = pulse;
        }
        else if (-(pulse - last_total_pulse) > encoder_step_threshold)
        {
            encoderUpdateDownFlag = true;
            last_total_pulse = pulse;
        }
        return;  // 调参模式不控制电机
    }

    // ---- Mode 2: 自动循迹（三串级PID）----
    if (program_exiting) return;

    // 第一层：角速度环（方向环输出作为目标角速度）
    imu_gyro_z = imu_dev.get_gyro_z();
    pid_gyroz->Kp = (std::abs(error0) >= 15.0f) ? (p_gyroz + 0.06f) : p_gyroz;
    corr_gyroz = pid_gyroz->compute(imu_gyro_z, -corr_position);

    // 第二层：速度环（编码器反馈）
    encoder_get();

    // 读取NCNN状态（预留）
    uint8_t ncnn_code = read_ncnn_state();
    static uint8_t last_ncnn_code = 0xFF;
    if (ncnn_code != last_ncnn_code) {
        last_ncnn_code = ncnn_code;
    }

    // 绕行状态保存/恢复机制
    static struct {
        int cross_flag, left_cirque, right_cirque;
        float v_target;
        bool saved;
    } pre_turn = {0, 0, 0, 530.0f, false};
    static uint8_t last_strategy = 0;

    if (strategy_state == 1 && last_strategy == 0)
    {
        pre_turn.cross_flag = elementDetector.Cross_Flag;
        pre_turn.left_cirque  = elementDetector.LeftCirque_Flag;
        pre_turn.right_cirque = elementDetector.RightCirque_Flag;
        pre_turn.v_target     = v_target;
        pre_turn.saved        = true;
        elementDetector.RightCirque_Flag = 0;
        elementDetector.LeftCirque_Flag  = 0;
        elementDetector.Cross_Flag = 0;
    }
    else if (strategy_state == 0 && last_strategy == 1 && pre_turn.saved)
    {
        elementDetector.Cross_Flag = pre_turn.cross_flag;
        elementDetector.LeftCirque_Flag  = pre_turn.left_cirque;
        elementDetector.RightCirque_Flag = pre_turn.right_cirque;
        v_target = pre_turn.v_target;
        pre_turn.saved = false;
    }
    last_strategy = strategy_state;

    // 第三层：速度环PID + 电机输出
    if (pid_left_ctrl && pid_right_ctrl)
    {
        // 计算左右轮目标速度（含角速度环差速修正）
        if (elementDetector.Straight_Flag)
        {
            v_target_left  = v_target - corr_gyroz;
            v_target_right = v_target + corr_gyroz;
        }
        else
        {
            v_target_left  = v_target - corr_gyroz;
            v_target_right = v_target + corr_gyroz;
        }

        // 增量式PID
        corr_l = pid_left_ctrl->compute(v_target_left, v_left);
        corr_r = pid_right_ctrl->compute(v_target_right, v_right);

        // 累加修正并限幅
        duty_left  = ::clamp(duty_left + corr_l, -MAX_DUTY, MAX_DUTY);
        duty_right = ::clamp(duty_right + corr_r, -MAX_DUTY, MAX_DUTY);

        // 调试打印（每360ms一次）
        static int tick_pid = 0;
        tick_pid++;
        if (tick_pid % 120 == 0)
        {
            printf("error0=%.2f imu=%d vt_l=%.1f vt_r=%.1f | corr_l=%d corr_r=%d duty_l=%d duty_r=%d\n",
                   error0, imu_gyro_z, v_target_left, v_target_right,
                   corr_l, corr_r, duty_left, duty_right);
        }

        motor_right_output(duty_right);
        motor_left_output(duty_left);
    }

    // TOF测距（预留）
    if (NO_FIND_DEVICE != dl1x_dev_type)
        dl1x_distance_raw = dl1x_dev.get_distance();
}

// ============================================
// region 清理函数（cap已声明，可正常调用）
// ============================================

void cleanup()
{
    printf("odometer=%.3f Cross=%d RC=%d LC=%d\n",
           odometer.total_pulse, elementDetector.Cross_Flag,
           elementDetector.RightCirque_Flag, elementDetector.LeftCirque_Flag);
    printf("程序退出，执行清理\n");

    // 保存参数
    if (!saveParams("input.txt"))
        cout << "保存参数失败！" << endl;

    // 闭环减速停车（柔和停车）
    int max_attempts = 100;
    v_target_left = v_target_right = 0;
    for (int i = 0; i < max_attempts; ++i)
    {
        encoder_get();
        if (pid_left_ctrl && pid_right_ctrl)
        {
            corr_l = pid_left_ctrl->compute(v_target_left, v_left);
            corr_r = pid_right_ctrl->compute(v_target_right, v_right);
            duty_left  = ::clamp(duty_left + corr_l, -MAX_DUTY, MAX_DUTY);
            duty_right = ::clamp(duty_right + corr_r, -MAX_DUTY, MAX_DUTY);
            motor_left_output(duty_left);
            motor_right_output(duty_right);
        }
        if ((abs(v_left) < 2) && (abs(v_right) < 2))
        {
            printf("速度已接近为零，提前退出\n");
            break;
        }
        system_delay_ms(5);
    }

    // 关闭硬件
    drv8701e_pwm_1.set_duty(0);
    drv8701e_pwm_2.set_duty(0);
    esc_pwm.set_duty(0);
    beep.set_level(0);

    // 释放摄像头（cap已声明，不会报错）
    cap.release();

    // 清零状态
    odometer.total_pulse = 0;
    elementDetector.Cross_Flag = 0;
    elementDetector.LeftCirque_Flag = 0;
    elementDetector.RightCirque_Flag = 0;
    elementDetector.Straight_Flag = 0;

    pit_control.stop();
}

// ============================================
// region 信号处理
// ============================================

void sigint_handler(int signum)
{
    program_exiting = true;
    printf("收到Ctrl+C\n");
    system_delay_ms(50);
    cleanup();
    exit(0);
}

// ============================================
// region 主函数
// ============================================

int main(int, char**)
{
    // ----- 硬件初始化 -----
    drv8701e_pwm_1.get_dev_info(&drv8701e_pwm_1_info);
    drv8701e_pwm_2.get_dev_info(&drv8701e_pwm_2_info);
    esc_pwm.get_dev_info(&esc_info);
    ips.init("/dev/fb0");
    imu_dev.init();

    dl1x_dev_type = dl1x_dev.init();
    if (NO_FIND_DEVICE == dl1x_dev_type)
        printf("[WARN] DL1X 未找到\r\n");
    else
        printf("DL1X 初始化成功\r\n");

    encoder_quad_2.clear_count();

    // ----- 加载参数 -----
    if (!readParams("input.txt")) return 1;

    // ----- 创建PID控制器 -----
    pid_left_ctrl  = new IncrementalPID(p_l, i_l, d_l);
    pid_right_ctrl = new IncrementalPID(p_r, i_r, d_r);
    pid_position   = new PositionPID(p_position, d_position);
    pid_gyroz      = new GyrozPID(p_gyroz, d_gyroz);

    Menu->updateDisplay();

    // ----- 注册信号 -----
    signal(SIGINT, sigint_handler);

    // ----- 启动PIT定时器（3ms周期）-----
    pit_control.init_ms(3, pit_callback);

    // ----- 主循环状态机 -----
    while (1)
    {
        Mode_Old = Mode;
        Mode = gpio_switch.get_level() ? 2 : 1;

        if (Mode ^ Mode_Old)
        {
            if (Mode == 2)
                printf("切换到运行模式（视觉自主导航）\n");
            else
            {
                runFlag = false;
                program_exiting = true;
                printf("切换到调参模式\n");
            }
        }

        switch (Mode)
        {
            // ---- Mode 1: 菜单调参 ----
            case 1:
            {
                uint8_t keyValue = key_scan();
                if ((keyValue ^ keyOld) & keyValue)
                {
                    Menu->handleInput(keyValue);
                    Menu->updateDisplay();
                }
                keyOld = keyValue;
                system_delay_ms(20);
            }
            break;

            // ---- Mode 2: 视觉自主导航 ----
            case 2:
            {
                if (runFlag == true)
                {
                    // 斑马线自动停车（起步3米后生效）
                    if (odometer.total_pulse > 3)
                    {
                        if (zebra_meter == 0)
                        {
                            if (elementDetector.Zebra_crossing_flag)
                                zebra_meter = odometer.total_pulse;
                        }
                        else
                        {
                            if (zebra_meter + 0.5 < odometer.total_pulse)
                            {
                                printf("检测到斑马线，触发退出\n");
                                program_exiting = true;
                                runFlag = false;
                                cleanup();
                                zebra_meter = 0;
                            }
                        }
                    }

                    // 板载按键紧急停车（KEY_LEFT）
                    uint8_t keyValue = key_scan();
                    if ((keyValue ^ keyOld) & keyValue)
                    {
                        if (keyValue == KEY_LEFT)
                        {
                            printf("按键触发紧急停车\n");
                            program_exiting = true;
                            runFlag = false;
                            cleanup();
                        }
                    }
                    keyOld = keyValue;

                    // 图像处理与视觉导航
                    if (cap.isOpened() && !program_exiting)
                    {
                        // 1. 阻塞读取摄像头帧
                        if (!cap.read(frame)) { continue; }
                        if (frame.empty()) continue;

                        // 2. 缩放至处理分辨率
                        cv::Mat img_120;
                        cv::resize(frame, img_120, cv::Size(USED_COL, USED_ROW), 0, 0, cv::INTER_LINEAR);

                        // 3. 转灰度
                        if (img_120.channels() == 3)
                            cvtColor(img_120, gray, COLOR_BGR2GRAY);
                        else
                            gray = img_120;

                        // 4. 视觉管线：IPM → 边线 → 元素 → 中线
                        runVisionPipeline(gray, preprocess, lineTracker, elementDetector, centerLine, yolo);

                        // 5. 获取导航偏差（中线偏移，像素）
                        error0 = centerLine.getError();
                        bool in_cirque_state4 = (elementDetector.LeftCirque_Flag == 4 ||
                                                 elementDetector.RightCirque_Flag == 4 ||
                                                 elementDetector.LeftCirque_Flag == 6 ||
                                                 elementDetector.RightCirque_Flag == 6);
                        if (error0 > 16 && in_cirque_state4) error0 = 16;
                        else if (error0 < -16 && in_cirque_state4) error0 = -16;

                        // 6. 方向环（位置式PD）：error0 → corr_position
                        corr_position = pid_position->compute(static_cast<float>(error0));

                        // 动态Kp：圆环/十字状态增强响应
                        pid_position->Kp = (
                            elementDetector.RightCirque_Flag == 4 ||
                            elementDetector.RightCirque_Flag == 5 ||
                            elementDetector.LeftCirque_Flag == 4 ||
                            elementDetector.LeftCirque_Flag == 5)
                            ? (p_position + Cirque_Flag_kp) : p_position;
                        pid_position->Kp = (elementDetector.Cross_Flag >= 1)
                            ? (p_position + Cross_Flag_Kp) : p_position;

                        // 7. 元素状态变化检测 → 蜂鸣器 + 存图
                        int cross_flag = elementDetector.Cross_Flag;
                        int left_cirque_flag = elementDetector.LeftCirque_Flag;
                        int right_cirque_flag = elementDetector.RightCirque_Flag;
                        int l_obstacle_flag = elementDetector.R_Obstacle_flag;
                        int r_obstacle_flag = elementDetector.L_Obstacle_flag;
                        int slope_flag = elementDetector.Slope_flag;

                        bool state_changed = (cross_flag != last_cross_flag) ||
                                             (left_cirque_flag != last_left_cirque_flag) ||
                                             (right_cirque_flag != last_right_cirque_flag) ||
                                             (l_obstacle_flag != last_l_obstacle_flag) ||
                                             (r_obstacle_flag != last_r_obstacle_flag) ||
                                             (slope_flag != last_slope_flag);

                        if (state_changed)
                        {
                            beep_countdown = 20;
                            if (elementDetector.LeftCirque_Flag == 2 || elementDetector.RightCirque_Flag == 2)
                                cv::imwrite("/home/root/original_cap.jpg", preprocess.getIPMImage());
                            if (elementDetector.LeftCirque_Flag == 4 || elementDetector.RightCirque_Flag == 4)
                                cv::imwrite("/home/root/original_cap4.jpg", preprocess.getIPMImage());
                            if (elementDetector.LeftCirque_Flag == 5 || elementDetector.RightCirque_Flag == 5)
                                cv::imwrite("/home/root/original_cap5.jpg", preprocess.getIPMImage());
                            if (elementDetector.LeftCirque_Flag == 6 || elementDetector.RightCirque_Flag == 6)
                                cv::imwrite("/home/root/original_cap6.jpg", preprocess.getIPMImage());
                            if (elementDetector.Cross_Flag == 4)
                                cv::imwrite("/home/root/original_capc.jpg", preprocess.getIPMImage());
                            if (elementDetector.Cross_Flag == 1)
                                cv::imwrite("/home/root/original_capc1.jpg", preprocess.getIPMImage());
                            if (elementDetector.Cross_Flag == 5)
                                cv::imwrite("/home/root/original_capc5.jpg", preprocess.getIPMImage());
                            if (elementDetector.Slope_flag)
                                cv::imwrite("/home/root/original_caps.jpg", preprocess.getIPMImage());
                            if (elementDetector.R_Obstacle_flag || elementDetector.L_Obstacle_flag)
                                cv::imwrite("/home/root/original_capo.jpg", preprocess.getIPMImage());
                        }

                        if (beep_countdown > 0) {
                            beep.set_level(1);
                            beep_countdown--;
                        } else {
                            beep.set_level(0);
                        }

                        last_cross_flag = cross_flag;
                        last_left_cirque_flag = left_cirque_flag;
                        last_right_cirque_flag = right_cirque_flag;
                        last_r_obstacle_flag = r_obstacle_flag;
                        last_l_obstacle_flag = l_obstacle_flag;
                        last_slope_flag = slope_flag;
                    }

                    system_delay_ms(5);
                }
                else
                {
                    // 等待发车（KEY_RIGHT）
                    uint8_t keyValue = key_scan();
                    if ((keyValue ^ keyOld) & keyValue)
                    {
                        if (keyValue == KEY_RIGHT)
                        {
                            cap.open(CAM_PATH);
                            if (!cap.isOpened())
                            {
                                std::cerr << "摄像头初始化失败！" << std::endl;
                                return -1;
                            }
                            cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
                            cap.set(cv::CAP_PROP_FRAME_WIDTH,  CAM_WIDTH);
                            cap.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);
                            cap.set(cv::CAP_PROP_FPS, CAM_FPS);
                            cap.set(cv::CAP_PROP_AUTO_EXPOSURE, CAM_AUTO_EXPOSURE_DISABLE);
                            cap.set(cv::CAP_PROP_EXPOSURE, 400);

                            printf("cam opened: %dx%d @ %.0f fps\n",
                                   (int)cap.get(cv::CAP_PROP_FRAME_WIDTH),
                                   (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT),
                                   cap.get(cv::CAP_PROP_FPS));

                            system_delay_ms(500);

                            uint16_t delayMs = ((duty_left_press + duty_right_press) / 2 - 500.0f) > 0
                                               ? static_cast<uint16_t>((duty_left_press + duty_right_press) / 2 - 500.0f) + 500U
                                               : 1U;

                            printf("已发车!\n");
                            runFlag = true;
                            esc_pwm.set_duty(static_cast<uint16_t>(duty_right_press));
                            system_delay_ms(delayMs);
                            program_exiting = false;
                        }
                    }
                    keyOld = keyValue;
                    system_delay_ms(10);
                }
            }
            break;
        }
    }

    return 0;
}