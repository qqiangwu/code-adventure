//
// Created by wuqq on 23/01/2018.
//

#ifndef EXPR_ASYNC_LOGGER_H
#define EXPR_ASYNC_LOGGER_H

#include <cstdio>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/circular_buffer.hpp>

namespace Logging {
    /**
     * TODO 问题：如何选择Buffer的大小？
     */
    class Async_logger : private boost::noncopyable {
    public:
        static constexpr int buffer_count = 8;
        static constexpr int default_buffer_size = 4 * 1024 * 1024;

        /**
         * @param file_sink
         * @param duration
         * @param buffer_size
         * @throw std::system_error if cannot open file for append or worker failed to start
         * @pre   buffer_size > 0
         */
        Async_logger(const std::string& file_sink,
                const std::chrono::milliseconds duration,
                const int buffer_size = default_buffer_size);

        /**
         * file will be closed silently since it's not a fatal error as a log file.
         */
        ~Async_logger() noexcept;

        const int buffer_size() const noexcept
        {
            return buffer_size_;
        }

        /**
         * append log entry
         * @param line
         * @pre line.size <= buffer_size()
         *
         * Writing is performed in the background. If bg writing failed, abort.
         */
        void append(const std::string& line) noexcept;

    private:
        using Buffer_ptr = std::string*;

        void do_work_() noexcept;

        bool can_write_(const std::string& line) const noexcept;

        Buffer_ptr poll_writable_buffer_() noexcept;

        void make_buffer_usable(Buffer_ptr buffer) noexcept;

    private:
        const int buffer_size_;
        std::FILE* out_;
        std::chrono::milliseconds flush_interval_;
        boost::circular_buffer<Buffer_ptr> backend_buffers_ { buffer_count };
        boost::circular_buffer<Buffer_ptr> frontend_buffers_ { buffer_count };
        std::atomic<bool> stopped_;
        std::thread worker_;
        std::mutex  mutex_;
        std::condition_variable frontend_usable_;
        std::condition_variable backend_usable_;
    };
}

#endif //EXPR_ASYNC_LOGGER_H
