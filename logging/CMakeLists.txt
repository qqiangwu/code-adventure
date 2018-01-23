file(GLOB headers "*.h")
file(GLOB impls "*.cpp")

add_executable(logging ${headers} ${impls})
target_include_directories(logging PRIVATE ${Boost_INCLUDE_DIRS})