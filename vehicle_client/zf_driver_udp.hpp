#ifndef __ZF_DRIVER_UDP_HPP__
#define __ZF_DRIVER_UDP_HPP__

#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cerrno>

typedef int8_t   int8;
typedef uint8_t  uint8;
typedef uint32_t uint32;
typedef uint32_t u32;
typedef int32_t  int32;

class zf_driver_udp
{
private:
    int                 m_socket;
    struct sockaddr_in  m_server_addr;
    socklen_t           m_server_addr_size;

    zf_driver_udp(const zf_driver_udp&) = delete;
    zf_driver_udp& operator=(const zf_driver_udp&) = delete;

public:
    zf_driver_udp(void);
    ~zf_driver_udp(void);

    int8 init(const char *ip_addr, uint32 port);
    uint32 send_data(const uint8 *buff, uint32 length);
    uint32 read_data(uint8 *buff, uint32 length);
};

#endif
