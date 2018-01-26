#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "acceptor.h"
#include "error_util.h"

using namespace Net;

Acceptor::Acceptor(Ipv4_addr addr)
    : addr_(addr), fd_(::socket(AF_INET, SOCK_STREAM, 0))
{
    if (fd_ < 0) {
        throw_system_error();
    }

    int enable_reuse_addr = 1;
    auto r = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &enable_reuse_addr, sizeof(enable_reuse_addr));
    if (r < 0) {
        ::close(fd_);
        throw_system_error();
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(addr.ip().value());
    server_addr.sin_port = htons(addr.port());

    r = ::bind(fd_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
    if (r < 0) {
        ::close(fd_);
        throw_system_error();
    }

    r = ::listen(fd_, 1024);
    if (r < 0) {
        ::close(fd_);
        throw_system_error();
    }
}

Acceptor::~Acceptor()
{
    assert(fd_ >= 0);

    ::close(fd_);
}

std::unique_ptr<Socket> Acceptor::accept()
{
    sockaddr_in remote {};
    socklen_t len {};
    const int fd = ::accept(fd_, reinterpret_cast<sockaddr*>(&remote), &len);
    if (fd >= 0) {
        Ip ip { ntohl(remote.sin_addr.s_addr) };
        Ipv4_addr remote_ { ip, ntohs(remote.sin_port) };

        return std::unique_ptr<Socket>(new Socket(fd, addr_, remote_));
    } else {
        if (fd == EAGAIN || fd == EWOULDBLOCK || fd == EINTR) {
            return nullptr;
        }

        throw_system_error();
    }
}
