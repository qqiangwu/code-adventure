#include <system_error>
#include <cerrno>

inline void throw_system_error()
{
    throw std::system_error(std::error_code(errno, std::system_category()));
}
