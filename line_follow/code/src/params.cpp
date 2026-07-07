#include "../inc/params.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

using namespace std;

// PID参数
extern float p_l;
extern float i_l;
extern float d_l;
extern float p_r;
extern float i_r;
extern float d_r;
extern float v_target;
extern float p_position;
extern float d_position;
extern float p_gyroz;
extern float d_gyroz;
extern float overspeed;

// PWM预设占空比
extern float duty_left_press;
extern float duty_right_press;


// // LADRC参数
// extern float ladrc_l_wc;
// extern float ladrc_l_w0;
// extern float ladrc_l_b0;
// extern float ladrc_l_r;
// extern float ladrc_r_wc;
// extern float ladrc_r_w0;
// extern float ladrc_r_b0;
// extern float ladrc_r_r;

// 读取参数函数
bool readParams(const std::string& filename)
{
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "无法打开文件: " << filename << endl;
        return false;
    }

    string line;
    regex pattern(R"((p_l|i_l|d_l|p_r|i_r|d_r|v_target|p_position|d_position|p_gyroz|d_gyroz|duty_left_press|duty_right_press|overspeed):\s*(-?\d+(?:\.\d+)?))");
    // regex pattern(R"((ladrc_l_wc|ladrc_l_w0|ladrc_l_b0|ladrc_l_r|ladrc_r_wc|ladrc_r_w0|ladrc_r_b0|ladrc_r_r|p_position|d_position|p_gyroz|d_gyroz|duty_left_press|duty_right_press|overspeed):\s*(-?\d+(?:\.\d+)?))");
    smatch match;

    while (getline(file, line)) {
        if (regex_search(line, match, pattern)) {
            string param = match[1];
            float value = stof(match[2]);
            //
            if      (param == "p_l") p_l = value;
            else if (param == "i_l") i_l = value;
            else if (param == "d_l") d_l = value;
            else if (param == "p_r") p_r = value;
            else if (param == "i_r") i_r = value;
            else if (param == "d_r") d_r = value;
            // if (param == "ladrc_l_wc") ladrc_l_wc = value;
            // else if (param == "ladrc_l_w0") ladrc_l_w0 = value;
            // else if (param == "ladrc_l_b0") ladrc_l_b0 = value;
            // else if (param == "ladrc_l_r")  ladrc_l_r  = value;
            // else if (param == "ladrc_r_wc") ladrc_r_wc = value;
            // else if (param == "ladrc_r_w0") ladrc_r_w0 = value;
            // else if (param == "ladrc_r_b0") ladrc_r_b0 = value;
            // else if (param == "ladrc_r_r")  ladrc_r_r  = value;
            else if (param == "v_target") v_target = value;
            else if (param == "p_position") p_position = value;
            else if (param == "d_position") d_position = value;
            else if (param == "p_gyroz") p_gyroz = value;
            else if (param == "d_gyroz") d_gyroz = value;
            else if (param == "duty_left_press") duty_left_press = value;
            else if (param == "duty_right_press") duty_right_press = value;
            else if (param == "overspeed") overspeed = value;
        } else {
            cerr << "未能匹配：" << line << endl;
        }
    }

    file.close();
    return true;
}

// 保存参数函数
bool saveParams(const std::string& filename)
{
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "无法打开文件: " << filename << endl;
        return false;
    }

    file << "p_l: " << p_l << endl;
    file << "i_l: " << i_l << endl;
    file << "d_l: " << d_l << endl;
    file << "p_r: " << p_r << endl;
    file << "i_r: " << i_r << endl;
    file << "d_r: " << d_r << endl;
    // file << "ladrc_l_wc: " << ladrc_l_wc << endl;
    // file << "ladrc_l_w0: " << ladrc_l_w0 << endl;
    // file << "ladrc_l_b0: " << ladrc_l_b0 << endl;
    // file << "ladrc_l_r: "  << ladrc_l_r  << endl;
    // file << "ladrc_r_wc: " << ladrc_r_wc << endl;
    // file << "ladrc_r_w0: " << ladrc_r_w0 << endl;
    // file << "ladrc_r_b0: " << ladrc_r_b0 << endl;
    // file << "ladrc_r_r: "  << ladrc_r_r  << endl;
    file << "v_target: " << v_target << endl;
    file << "p_position: " << p_position << endl;
    file << "d_position: " << d_position << endl;
    file << "p_gyroz: " << p_gyroz << endl;
    file << "d_gyroz: " << d_gyroz << endl;
    file << "duty_left_press: " << duty_left_press << endl;
    file << "duty_right_press: " << duty_right_press << endl;
    file << "overspeed: " << overspeed << endl;

    file.close();
    return true;
}


