cmake_minimum_required(VERSION 3.28)
project(chan_cpp)

set(CMAKE_CXX_STANDARD 20)

# for closing Cmake 3.28 new module scan feature
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(thirdparty/googletest-1.15.2)

add_executable(chan_cpp main.cpp
)

target_link_libraries(chan_cpp PRIVATE chan_cpp_headers)

target_include_directories(chan_cpp PRIVATE ${CMAKE_SOURCE_DIR}/include)
