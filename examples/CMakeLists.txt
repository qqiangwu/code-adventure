add_executable(net_iterative_server net/iterative_server.cpp)
target_link_libraries(net_iterative_server PRIVATE net)

add_executable(net_echo net/echo.cpp)
target_link_libraries(net_echo PRIVATE net)

add_executable(net_pingpong net/pingpong.cpp)
target_link_libraries(net_pingpong PRIVATE net)