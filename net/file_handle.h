//
// Created by wuqq on 28/01/2018.
//

#ifndef EXPR_FILE_HANDLE_H
#define EXPR_FILE_HANDLE_H

#include <algorithm>
#include <cassert>
#include "common_util.h"

namespace Net {
    using Native_handle_type = int;

    namespace Detail {
        class File_handle : private Noncopyable {
        public:
            static constexpr Native_handle_type invalid_file_handle = -1;

            File_handle(File_handle&& other) noexcept
                    : fd_(other.fd_)
            {
                other.fd_ = invalid_file_handle;
            }

            File_handle& operator=(File_handle&& other) noexcept
            {
                std::swap(fd_, other.fd_);

                return *this;
            }

            Native_handle_type handle() const noexcept
            {
                return fd_;
            }

        public:
            bool is_nonblocking() const noexcept
            {
                return nonblocking_;
            }

            /**
             * @param nonblocking
             */
            void set_nonblocking(const bool nonblocking) noexcept;

        protected:
            explicit File_handle(Native_handle_type fd) noexcept
                    : fd_(fd)
            {
                assert(fd >= 0);
            }

            ~File_handle() noexcept;

        private:
            Native_handle_type fd_;
            bool nonblocking_ { false };
        };
    }
}

#endif //EXPR_FILE_HANDLE_H
