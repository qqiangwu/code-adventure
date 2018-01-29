#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include "socket.h"
#include "error_util.h"

using namespace Net;

int Socket::read(std::vector<char>& buffer) noexcept
{
    int bytes_read = 0;

    while (bytes_read < buffer.size()) {
        char* start = buffer.data() + bytes_read;
        const int remaining = buffer.size() - bytes_read;

        const auto rc = ::read(handle(), start, remaining);

        if (rc == 0) {
            break;
        } else if (rc < 0) {
            assert(errno != EBADF);
            assert(errno != EFAULT);
            assert(errno != EINVAL);

            if (errno != EINTR) {
                break;
            }
        }

        bytes_read += rc;
    }

    return bytes_read > 0? bytes_read: -1;
}

int Socket::write(const std::vector<char>& buffer) noexcept
{
    int bytes_written = 0;

    while (bytes_written < buffer.size()) {
        const char* start = buffer.data() + bytes_written;
        const int remaining = buffer.size() - bytes_written;

        const auto rc = ::write(handle(), start, remaining);

        if (rc == 0) {
            break;
        } else if (rc < 0) {
            assert(errno != EBADF);
            assert(errno != EFAULT);
            assert(errno != EINVAL);

            if (errno != EINTR) {
                break;
            }
        }

        bytes_written += rc;
    }

    return bytes_written > 0? bytes_written: -1;
}

int Socket::read_some(std::vector<char>& buffer) noexcept
{
    for (;;) {
        const int rc = ::read(handle(), buffer.data(), buffer.size());

        if (rc >= 0) {
            return rc;
        } else {
            assert(errno != EBADF);
            assert(errno != EFAULT);
            assert(errno != EINVAL);

            if (errno != EINTR) {
                return rc;
            }
        }

    }
}

int Socket::write_some(const std::vector<char>& buffer) noexcept
{
    for (;;) {
        const int rc = ::write(handle(), buffer.data(), buffer.size());

        if (rc >= 0) {
            return rc;
        } else {
            assert(errno != EBADF);
            assert(errno != EFAULT);
            assert(errno != EINVAL);

            if (errno != EINTR) {
                return rc;
            }
        }
    }
}