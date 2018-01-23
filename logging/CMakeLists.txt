set(LOGGING_SRC async_logger.h async_logger.cpp)

add_executable(logging_throughput_bench throughput_bench.cpp ${LOGGING_SRC})
target_include_directories(logging_throughput_bench PRIVATE ${Boost_INCLUDE_DIRS})
if(CMAKE_COMPILER_IS_GNUCC)
    target_link_libraries(logging_throughput_bench PRIVATE pthread)
endif()

add_executable(logging_buffer_size_bench buffer_size_bench.cpp ${LOGGING_SRC})
target_include_directories(logging_buffer_size_bench PRIVATE ${Boost_INCLUDE_DIRS})
if(CMAKE_COMPILER_IS_GNUCC)
    target_link_libraries(logging_buffer_size_bench PRIVATE pthread)
endif()
