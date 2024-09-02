add_library(chan_cpp_impl INTERFACE)

target_include_directories(chan_cpp_impl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(chan_cpp_impl INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/channel.ccpp
        ${CMAKE_CURRENT_SOURCE_DIR}/chan_utitlity.cpp
)
