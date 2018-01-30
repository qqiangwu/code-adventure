#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include "socket.h"
#include "net_error.h"

using namespace Net;

int Socket::read(std::vector<char>& buffer)
{
    assert(!is_nonblocking());

    int rc = -1;
    int bytes_read = 0;

    while (bytes_read < buffer.size()) {
        char* start = buffer.data() + bytes_read;
        const int remaining = buffer.size() - bytes_read;

        rc = ::read(handle(), start, remaining);

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

    if (bytes_read > 0) {
        return bytes_read;
    } else if (rc == 0) {
        return -1;
    } else {
        switch (errno) {
        case EAGAIN: return 0;
        case ETIMEDOUT: return -2;
        case ECONNRESET: throw_net_error<Connection_reset>();
        case ENOBUFS: case ENOMEM:
            throw_net_error<Resource_not_enough>();

        default:
            assert(false && "bad errno in read");
            break;
        }
    }
}

int Socket::write(const std::vector<char>& buffer)
{
    assert(!is_nonblocking());

    int bytes_written = 0;

    while (bytes_written < buffer.size()) {
        const char* start = buffer.data() + bytes_written;
        const int remaining = buffer.size() - bytes_written;

        const int rc = ::write(handle(), start, remaining);

        if (rc < 0) {
            assert(errno != EBADF);
            assert(errno != EFAULT);
            assert(errno != EINVAL);

            if (errno != EINTR) {
                break;
            }
        }

        bytes_written += rc;
    }

    if (bytes_written > 0) {
        return bytes_written;
    } else {
        switch (errno) {
        case EAGAIN: return 0;
        case ETIMEDOUT: return -2;
        case ECONNRESET: throw_net_error<Connection_reset>();
        case ENETDOWN: throw_net_error<Net_down>();
        case ENETUNREACH: throw_net_error<Net_unreachable>(remote_addr());
        case ENOTSOCK: throw_net_error<Resource_not_enough>();

        default:
            assert(false && "bad errno in write");
            break;
        }
    }
}

int Socket::read_some(std::vector<char>& buffer)
{
    for (;;) {
        const int rc = ::read(handle(), buffer.data(), buffer.size());

        if (rc > 0) {
            return rc;
        } else if (rc == 0) {
            return -1;
        } else {
            assert(errno != EBADF);
            assert(errno != EFAULT);
            assert(errno != EINVAL);

            if (errno != EINTR) {
                switch (errno) {
                case EAGAIN: return 0;
                case ETIMEDOUT: return -2;
                case ECONNRESET: throw_net_error<Connection_reset>();
                case ENOBUFS: case ENOMEM:
                    throw_net_error<Resource_not_enough>();
                case EPIPE: throw_net_error<Remote_closed>();

                default:
                    assert(false && "bad errno in read_some");
                    break;
                }
            }
        }

    }
}

int Socket::write_some(const std::vector<char>& buffer)
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
                switch (errno) {
                case EAGAIN: return 0;
                case ETIMEDOUT: return -2;
                case ECONNRESET: throw_net_error<Connection_reset>();
                case ENETDOWN: throw_net_error<Net_down>();
                case ENETUNREACH: throw_net_error<Net_unreachable>(remote_addr());
                case ENOTSOCK: throw_net_error<Resource_not_enough>();
                case EPIPE: throw_net_error<Remote_closed>();

                default:
                    assert(false && "bad errno in write_some");
                    break;
                }
            }
        }
    }
}