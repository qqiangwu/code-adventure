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
     * TODO 问题: /dev/null非常慢
     * TODO 问题：如何安全关闭，因为关闭时条件变量可能正在等待
     */
    class Async_logger : private boost::noncopyable {
    public:
        static constexpr int buffer_count = 8;
        static constexpr int max_line_size = 4 * 1024 * 1024;

        /**
         * @param file_sink
         * @param duration
         * @throw std::system_error if cannot open file for append or worker failed to start
         */
        Async_logger(const std::string& file_sink, const std::chrono::milliseconds duration);

        /**
         * file will be closed silently since it's not a fatal error as a log file.
         */
        ~Async_logger() noexcept;

        /**
         * append log entry
         * @param line
         * @pre line.size <= max_line_size
         *
         * Writing is performed in the background. If bg writing failed, abort.
         */
        void append(const std::string& line) noexcept;

    private:
        using Buffer_ptr = std::string*;

        void do_work_() noexcept;

        bool can_write_(const std::string& line) noexcept;

        Buffer_ptr poll_writable_buffer_() noexcept;

        void make_buffer_usable(Buffer_ptr buffer) noexcept;

    private:
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
