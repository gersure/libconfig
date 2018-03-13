#pragma once

#include <iosfwd>
#include <sys/socket.h>
#include <netinet/ip.h>
#include "byteorder.hh"

namespace core {

struct ipv4_addr;

class socket_address {
public:
    union {
        ::sockaddr_storage sas;
        ::sockaddr sa;
        ::sockaddr_in in;
    } u;
    socket_address(sockaddr_in sa) {
        u.in = sa;
    }
    socket_address(ipv4_addr);
    socket_address() = default;
    ::sockaddr& as_posix_sockaddr() { return u.sa; }
    ::sockaddr_in& as_posix_sockaddr_in() { return u.in; }
    const ::sockaddr& as_posix_sockaddr() const { return u.sa; }
    const ::sockaddr_in& as_posix_sockaddr_in() const { return u.in; }

    bool operator==(const socket_address&) const;
};

std::ostream& operator<<(std::ostream&, const socket_address&);

enum class transport {
    TCP = IPPROTO_TCP,
    SCTP = IPPROTO_SCTP
};


namespace net {
class inet_address;
}

struct listen_options {
    transport proto = transport::TCP;
    bool reuse_address = false;
    listen_options(bool rua = false)
        : reuse_address(rua)
    {}
};

struct ipv4_addr {
    uint32_t ip;
    uint16_t port;

    ipv4_addr() : ip(0), port(0) {}
    ipv4_addr(uint32_t ip, uint16_t port) : ip(ip), port(port) {}
    ipv4_addr(uint16_t port) : ip(0), port(port) {}
    ipv4_addr(const std::string &addr);
    ipv4_addr(const std::string &addr, uint16_t port);
    ipv4_addr(const net::inet_address&, uint16_t);

    ipv4_addr(const socket_address &sa) {
        ip = net::ntoh(sa.u.in.sin_addr.s_addr);
        port = net::ntoh(sa.u.in.sin_port);
    }

    ipv4_addr(socket_address &&sa) : ipv4_addr(sa) {}
};

}
