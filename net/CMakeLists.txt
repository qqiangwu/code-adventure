set(LIB_SRC
    common_fwd.h
    common_util.h
    net_error.h
    addr.h addr.cpp
    file_handle.h file_handle.cpp
    socket.h socket.cpp
    acceptor.h acceptor.cpp
    selector.h selector.cpp)

add_library(net STATIC ${LIB_SRC})
target_include_directories(net PUBLIC .)