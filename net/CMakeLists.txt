set(LIB_SRC
    error_util.h
    addr.h addr.cpp
    socket.h socket.cpp
    acceptor.h acceptor.cpp)

add_library(net STATIC ${LIB_SRC})

add_executable(net_iterative_server iterative_server.cpp)
target_link_libraries(net_iterative_server PRIVATE net)
