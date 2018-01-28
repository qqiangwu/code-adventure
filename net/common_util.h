#ifndef NET_UTIL_NONCOPABLE_H_
#define NET_UTIL_NONCOPABLE_H_

class Noncopyable {
public:
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;

protected:
    Noncopyable() noexcept = default;
    ~Noncopyable() = default;
};

template <class T>
using Observer_ptr = T*;

#endif
