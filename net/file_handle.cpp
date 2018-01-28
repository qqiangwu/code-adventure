//
// Created by wuqq on 28/01/2018.
//

#include <unistd.h>
#include <fcntl.h>
#include <cassert>
#include "error_util.h"
#include "file_handle.h"

using namespace Net::Detail;

File_handle::~File_handle() noexcept
{
    assert(fd_ != invalid_file_handle);

    ::close(fd_);

    fd_ = invalid_file_handle;
}

void File_handle::set_nonblocking(const bool nonblocking)
{
    if (nonblocking_ == nonblocking) {
        return;
    }

    const int flags = ::fcntl(fd_, F_GETFL, 0);
    if (flags == -1) {
        throw_system_error();
    }

    const auto newflags = nonblocking?
                          (flags | O_NONBLOCK):
                          (flags ^ O_NONBLOCK);
    const auto r = ::fcntl(fd_, F_SETFL, newflags);
    if (r == -1) {
        throw_system_error();
    }

    nonblocking_ = nonblocking;
}