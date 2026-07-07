#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <opencv2/opencv.hpp>
#include "zf_driver_udp.hpp"

#define CAMERA_DEVICE 0
#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240
#define CAMERA_FPS 30
#define TRANSMIT_FPS 20
#define PC_IP "172.20.10.3"
#define PC_UDP_PORT 8888
#define JPEG_QUALITY 80
#define UDP_CHUNK_SIZE 1400

int main() {
    cv::VideoCapture cap(CAMERA_DEVICE);
    if (!cap.isOpened()) {
        std::cerr << "[ERROR] 摄像头初始化失败" << std::endl;
        return -1;
    }
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(cv::CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
    cap.set(cv::CAP_PROP_FPS, CAMERA_FPS);
    cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
    cap.set(cv::CAP_PROP_EXPOSURE, 100);

    double actual_w = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    double actual_h = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    std::cout << "[INFO] 摄像头就绪 " << (int)actual_w << "x" << (int)actual_h << std::endl;

    zf_driver_udp udp;
    if (udp.init(PC_IP, PC_UDP_PORT) != 0) {
        std::cerr << "[ERROR] UDP初始化失败" << std::endl;
        cap.release();
        return -1;
    }
    std::cout << "[INFO] UDP目标 " << PC_IP << ":" << PC_UDP_PORT << std::endl;

    cv::Mat frame;
    std::vector<uchar> jpeg_buf;
    std::vector<int> compress_params = {cv::IMWRITE_JPEG_QUALITY, JPEG_QUALITY};
    uint8_t pkt_buf[4 + UDP_CHUNK_SIZE];
    int frame_count = 0;
    const int sleep_ms = 1000 / TRANSMIT_FPS;

    std::cout << "[INFO] 开始传输 (Ctrl+C 停止)" << std::endl;

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "[WARN] 采集失败" << std::endl;
            usleep(sleep_ms * 1000);
            continue;
        }

        cv::imencode(".jpg", frame, jpeg_buf, compress_params);
        uint32_t total_size = jpeg_buf.size();
        uint32_t total_size_be = htonl(total_size);

        uint32_t offset = 0;
        while (offset < total_size) {
            uint32_t chunk_len = total_size - offset;
            if (chunk_len > UDP_CHUNK_SIZE)
                chunk_len = UDP_CHUNK_SIZE;

            memcpy(pkt_buf, &total_size_be, 4);
            memcpy(pkt_buf + 4, jpeg_buf.data() + offset, chunk_len);

            uint32_t sent = udp.send_data(pkt_buf, 4 + chunk_len);
            if (sent == 0) {
                std::cerr << "[WARN] UDP发送失败" << std::endl;
            }
            offset += chunk_len;
        }

        frame_count++;
        if (frame_count % 10 == 0) {
            uint32_t total_pkts = (total_size + UDP_CHUNK_SIZE - 1) / UDP_CHUNK_SIZE;
            std::cout << "[INFO] 第" << frame_count << "帧 "
                      << total_size << "B " << total_pkts << "包" << std::endl;
        }

        usleep(sleep_ms * 1000);
    }

    cap.release();
    return 0;
}
