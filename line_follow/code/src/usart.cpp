#include "usart.hpp"
#include "vofa_function.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

/* 全局变量 */
int g_uart_fd = -1;

/* 接收线程 */
static pthread_t g_recv_thread;
static volatile int g_running = 0;

/**
 * @brief  接收线程 - 批量阻塞读，逐字节塞给状态机
 */
static void* recv_thread_func(void* arg)
{
    uint8_t buf[64];   // 批量缓冲区，一次最多收 64 字节
    int ret;
    (void)arg;

    while (g_running) {
        // 阻塞读：内核等有数据才唤醒，无需 usleep
        ret = read(g_uart_fd, buf, sizeof(buf));
        if (ret > 0) {
            printf("[UART] received %d bytes\n", ret);  // 临时加这一行
            for (int i = 0; i < ret; i++) {
                uartCMDRecv(buf[i]);
            }
        }
    }
    return NULL;
}

/**
 * @brief  初始化串口 + 启动接收线程
 */
int USART_Init(void)
{
    struct termios tty;

    // 去掉 O_NDELAY，让 read 阻塞
    g_uart_fd = open(VOFA_UART_DEVICE, O_RDWR | O_NOCTTY);
    if (g_uart_fd < 0) {
        perror("open uart failed");
        return -1;
    }

    // 确保阻塞模式
    int flags = fcntl(g_uart_fd, F_GETFL, 0);
    fcntl(g_uart_fd, F_SETFL, flags & ~O_NONBLOCK);

    tcgetattr(g_uart_fd, &tty);
    cfsetispeed(&tty, VOFA_UART_BAUDRATE);
    cfsetospeed(&tty, VOFA_UART_BAUDRATE);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL | INLCR);
    tty.c_oflag &= ~OPOST;

    tcsetattr(g_uart_fd, TCSANOW, &tty);
    tcflush(g_uart_fd, TCIOFLUSH);

    g_running = 1;
    pthread_create(&g_recv_thread, NULL, recv_thread_func, NULL);

    return 0;
}

/**
 * @brief  关闭串口
 */
void USART_DeInit(void)
{
    g_running = 0;
    if (g_uart_fd >= 0) {
        close(g_uart_fd);   // 关闭 fd，唤醒阻塞的 read，让线程退出
        g_uart_fd = -1;
    }
    pthread_join(g_recv_thread, NULL);
}

/**
 * @brief  发送单字节
 */
void uartSendByte(uint8_t c)
{
    if (g_uart_fd >= 0) {
        write(g_uart_fd, &c, 1);
    }
}

/**
 * @brief  发送数据数组
 */
void uartSendData(uint8_t* Array, uint8_t SIZE)
{
    if (g_uart_fd >= 0 && Array != NULL && SIZE > 0) {
        write(g_uart_fd, Array, SIZE);
    }
}