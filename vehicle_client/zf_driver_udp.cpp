#include "zf_driver_udp.hpp"

zf_driver_udp::zf_driver_udp()
{
    m_socket = -1;
    m_server_addr_size = sizeof(m_server_addr);
    memset(&m_server_addr, 0, sizeof(m_server_addr));
}

zf_driver_udp::~zf_driver_udp()
{
    if (m_socket >= 0)
    {
        close(m_socket);
        m_socket = -1;
    }
}

int8 zf_driver_udp::init(const char *ip_addr, uint32 port)
{
    m_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if (m_socket == -1)
    {
        printf("Failed to create udp socket\r\n");
        return -1;
    }

    memset(&m_server_addr, 0, sizeof(m_server_addr));
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_addr.s_addr = inet_addr(ip_addr);
    m_server_addr.sin_port = htons(port);

    return 0;
}

uint32 zf_driver_udp::send_data(const uint8 *buff, uint32 length)
{
    ssize_t send_len = sendto(m_socket, buff, length, 0,
                             (sockaddr*)&m_server_addr, sizeof(m_server_addr));
    if (send_len == -1)
    {
        printf("udp sendto() error, errno:%d\r\n", errno);
        return 0;
    }
    return send_len;
}

uint32 zf_driver_udp::read_data(uint8 *buff, uint32 length)
{
    m_server_addr_size = sizeof(m_server_addr);

    ssize_t str_len = recvfrom(m_socket, buff, length, MSG_DONTWAIT,
                              (struct sockaddr *)&m_server_addr, &m_server_addr_size);
    if (str_len == -1)
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            printf("recvfrom() error, errno:%d\r\n", errno);
        }
        return 0;
    }

    return str_len;
}
