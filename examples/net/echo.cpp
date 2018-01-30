//
// Created by wuqq on 28/01/2018.
//

#include <iostream>
#include <vector>
#include "acceptor.h"
#include "selector.h"

using namespace Net;

bool is_running = true;
std::vector<char> buffer(4096);
Selector selector;

bool handle_accept(Observer_ptr<Acceptor> acceptor) noexcept;
bool handle_input(Observer_ptr<Socket> socket) noexcept;

int main(const int argc, const char** argv)
{
    Acceptor acceptor(Ipv4_addr(Ip::any(), 8000));
    acceptor.set_nonblocking(true);

    selector.add_acceptor(&acceptor, &handle_accept);

    std::cout << "Echo server started\n";

    while (is_running) {
        selector.select(std::chrono::milliseconds::max());
    }

    return 0;
}

bool handle_accept(Observer_ptr<Acceptor> acceptor) noexcept
try {
    while (auto conn = acceptor->try_accept()) {
        // don't set blocking
        std::shared_ptr<Socket> shared_conn(std::move(conn));
        selector.add_readable_socket(shared_conn.get(), [shared_conn](auto socket){
            return handle_input(shared_conn.get());
        });
    }

    return true;
} catch (std::system_error& e) {
    std::cerr << e.what() << std::endl;

    is_running = false;
    return false;
}

bool handle_input(Observer_ptr<Socket> socket) noexcept
{
    std::fill(buffer.begin(), buffer.end(), 0);

    auto alive = false;
    const auto rc = socket->read_some(buffer);

    if (rc > 0) {
        buffer.resize(rc);

        alive = socket->write(buffer) > 0;
    }

    if (!alive) {
        std::cout << "Connection closed: " << socket->remote_addr().str() << std::endl;
    }

    return alive;
}
