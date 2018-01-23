//
// Created by wuqq on 23/01/2018.
//

#include <cassert>
#include <stdexcept>
#include <system_error>
#include "async_logger.h"

using namespace std;
using namespace Logging;

namespace {
    template <class CircularBuffer>
    auto pop_front(CircularBuffer& buffer) noexcept
    {
        assert(!buffer.empty());

        auto v = std::move(buffer.front());
        buffer.pop_front();

        return v;
    }
}

Async_logger::Async_logger(const std::string& file_sink,
        const std::chrono::milliseconds duration,
        const int buffer_size)
    : buffer_size_(buffer_size),
      out_(std::fopen(file_sink.c_str(), "a")),
      flush_interval_(duration),
      stopped_(false)
{
    assert(buffer_size > 0 && "buffer size should be larger than zero");

    if (!out_) {
        throw std::system_error(error_code(errno, system_category()));
    }

    for (int i = 0; i < buffer_count; ++i) {
        auto buffer = std::make_unique<std::string>();
        buffer->reserve(buffer_size_);
        frontend_buffers_.push_back(buffer.release());
    }

    worker_ = thread([this]{ this->do_work_(); });
}

Async_logger::~Async_logger() noexcept
{
    if (worker_.joinable()) {
        stopped_ = true;
        backend_usable_.notify_one();
        worker_.join();
    }

    if (out_) {
        std::fclose(out_);
        out_ = nullptr;
    }

    while (!frontend_buffers_.empty()) {
        delete frontend_buffers_.front();
        frontend_buffers_.pop_front();
    }
}

void Async_logger::append(const std::string& line) noexcept
{
    assert(line.size() <= buffer_size_ && "line too long");

    std::unique_lock<std::mutex> lock(mutex_);

    while (!can_write_(line)) {
        frontend_usable_.wait(lock);
    }

    auto& buffer = frontend_buffers_.front();
    if (buffer->size() + line.size() <= buffer->capacity()) {
        buffer->append(line.begin(), line.end());
    } else {
        backend_buffers_.push_back(std::move(buffer));
        frontend_buffers_.pop_front();

        frontend_buffers_.front()->append(line.begin(), line.end());
        backend_usable_.notify_one();
    }
}

bool Async_logger::can_write_(const std::string& line) const noexcept
{
    if (frontend_buffers_.size() > 2) {
        return true;
    } else if (frontend_buffers_.size() == 1) {
        const auto& buffer = frontend_buffers_.front();

        return buffer->size() + line.size() <= buffer->max_size();
    } else {
        return false;
    }
}

void Async_logger::do_work_() noexcept
{
    while (!stopped_) {
        auto buffer = poll_writable_buffer_();

        const auto n = fwrite(buffer->data(), 1, buffer->size(), out_);
        if (n != buffer->size()) {
            std::terminate();
        }
        if (fflush(out_) != 0) {
            std::terminate();
        }

        make_buffer_usable(std::move(buffer));
    }
}

Async_logger::Buffer_ptr Async_logger::poll_writable_buffer_() noexcept
{
    std::unique_lock<std::mutex> lock(mutex_);

    while (!stopped_ && backend_buffers_.empty()) {
        backend_usable_.wait_for(lock, flush_interval_);

        if (!frontend_buffers_.empty() && !frontend_buffers_.front()->empty()) {
            break;
        }
    }

    auto buffer = !backend_buffers_.empty()
                  ? pop_front(backend_buffers_)
                  : pop_front(frontend_buffers_);

    return buffer;
}

void Async_logger::make_buffer_usable(Async_logger::Buffer_ptr buffer) noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);

    frontend_buffers_.push_back(std::move(buffer));
    frontend_usable_.notify_all();
}
