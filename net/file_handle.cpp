//
// Created by wuqq on 28/01/2018.
//

#include <unistd.h>
#include <fcntl.h>
#include "file_handle.h"

using namespace Net::Detail;

File_handle::~File_handle() noexcept
{
    assert(fd_ != invalid_file_handle);

    ::close(fd_);

    fd_ = invalid_file_handle;
}

void File_handle::set_nonblocking(const bool nonblocking) noexcept
{
    if (nonblocking_ == nonblocking) {
        return;
    }

    const int flags = ::fcntl(fd_, F_GETFL, 0);
    assert(flags >= 0);

    const auto newflags = nonblocking?
                          (flags | O_NONBLOCK):
                          (flags ^ O_NONBLOCK);
    const auto r = ::fcntl(fd_, F_SETFL, newflags);
    assert(r >= 0);

    nonblocking_ = nonblocking;
}