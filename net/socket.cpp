#include <cerrno>
#include <unistd.h>
#include "socket.h"

using namespace Net;

Socket::~Socket()
{
    assert(fd_ >= 0);

    // ignore error codes since fd is already closed
    ::close(fd_);
}

int Socket::read(std::vector<char>& buffer) noexcept
{
    return ::read(fd_, buffer.data(), buffer.size());
}

int Socket::write(const std::vector<char>& buffer) noexcept
{
    return ::write(fd_, buffer.data(), buffer.size());
}
