set(LIB_SRC
    common_fwd.h
    common_util.h
    error_util.h
    addr.h addr.cpp
    file_handle.h file_handle.cpp
    socket.h socket.cpp
    acceptor.h acceptor.cpp
    selector.h selector.cpp)

add_library(net STATIC ${LIB_SRC})
target_include_directories(net PUBLIC .)

add_executable(net_iterative_server examples/iterative_server.cpp)
target_link_libraries(net_iterative_server PRIVATE net)

add_executable(net_echo examples/echo.cpp)
target_link_libraries(net_echo PRIVATE net)