//
// Created by wuqq on 23/01/2018.
//

#include <string>
#include "async_logger.h"

using namespace std::string_literals;
using namespace std::chrono_literals;
using namespace std::chrono;
using namespace Logging;

void benchmark(const std::string& file_sink, const int n)
{
    const auto line = "Hello 0123456789 abcdefghijklmnopqrstuvwxyz"s;
    const int bytes = n * line.size();
    const auto from = std::chrono::system_clock::now();

    {
        Async_logger logger(file_sink, 3s);

        for (int i = 0; i < n; ++i) {
            logger.append(line.c_str());
        }
    }

    const auto seconds = duration_cast<milliseconds>(system_clock::now() - from).count() / 1000.f;

    printf("%12s: %f seconds, %d bytes, %10.2f msg/s, %.2f MiB/s\n",
            file_sink.c_str(), seconds, bytes, n / seconds, bytes / seconds / (1024 * 1024));
}

int main(const int argc, const char* argv[])
{
    int n = 100;
    if (argc > 1) {
        n = std::stoi(argv[1]);
    }

    benchmark("/dev/null", n);
    benchmark("/tmp/log", n);
}