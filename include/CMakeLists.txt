add_library(chan_cpp_headers INTERFACE
)

target_include_directories(chan_cpp_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(chan_cpp_headers INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/channel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/chan_utitlity.h
        ${CMAKE_CURRENT_SOURCE_DIR}/IChannel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/OChannel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ChannelBuffer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/circular_buffer.h
)