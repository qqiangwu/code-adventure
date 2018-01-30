#include <iostream>
#include <vector>
#include "acceptor.h"

int main()
try {
    using namespace Net;

    std::vector<char> buffer(1024);
    Acceptor acceptor { Ipv4_addr(Ip::any(), 8000) };

    for (;;) {
        auto client = acceptor.accept();

        if (client) {
            std::cout << "recv from: " << client->remote_addr().str();

            buffer.resize(buffer.capacity());
            const auto r = client->read_some(buffer);
            if (r > 0) {
                buffer.resize(r);
                client->write(buffer);
            }
        }
    }

    return 0;
} catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
}
