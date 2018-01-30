//
// Created by wuqq on 30/01/2018.
//

#include <iostream>
#include <unordered_set>
#include <acceptor.h>
#include <selector.h>

using namespace Net;

class Pingpong {
public:
    static constexpr int buffer_size = 4096;
    /**
     * @param addr
     * @throws Net_error if failed to start
     */
    explicit Pingpong(Ipv4_addr addr)
        : acceptor_(addr), buffer_(buffer_size)
    {
        acceptor_.set_nonblocking(true);
        selector_.add_acceptor(&acceptor_, [this](auto acceptor){
            return this->on_accept_(acceptor);
        });
    }

    void run()
    {
        std::cout << "Pingpong started" << std::endl;

        while (is_running_) {
            using namespace std::chrono;

            selector_.select(milliseconds::max());
        }
    }

private:
    bool on_accept_(Observer_ptr<Acceptor> acceptor) noexcept
    try {
        while (auto socket = acceptor->try_accept()) {
            add_connection(std::move(socket));
        }

        return true;
    } catch (Net_error& e) {
        std::cerr << "Accept failed: " << e.what() << std::endl;

        is_running_ = false;

        return false;
    }

    bool on_read_(Observer_ptr<Socket> socket) noexcept
    try {
        buffer_.resize(buffer_size);

        const int n = socket->read_some(buffer_);

        assert(n != 0);

        if (n < 0) {
            remove_connection(socket);
        } else if (n > 0) {
            buffer_.resize(n);
            socket->write(buffer_);
        }

        return true;
    } catch (Net_error& e) {
        std::cerr << "Read socket failed: " << e.what() << std::endl;

        remove_connection(socket);

        return false;
    }

private:
    void add_connection(std::unique_ptr<Socket> socket) noexcept
    {
        std::cout << "New connection from: " << socket->remote_addr().str() << std::endl;

        selector_.add_readable_socket(socket.get(), [this](auto socket){
            return this->on_read_(socket);
        });
        connections_.insert(std::move(socket));
    }

    void remove_connection(Observer_ptr<Socket> socket) noexcept
    {
        std::cout << "Remove connection from: " << socket->remote_addr().str() << std::endl;

        selector_.remove_readable_socket(socket);

        auto iter = std::find_if(connections_.begin(), connections_.end(), [socket](auto& e){
            return e.get() == socket;
        });

        if (iter != connections_.end()) {
            connections_.erase(iter);
        }
    }

private:
    Acceptor acceptor_;
    Selector selector_;
    std::unordered_set<std::unique_ptr<Socket>> connections_;
    bool is_running_ = true;
    std::vector<char> buffer_;
};

int main()
{
    Pingpong pingpong(Ipv4_addr(Ip::any(), 8000));

    pingpong.run();
}