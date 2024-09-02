add_executable(chan_cpp_tests
        basic_test.cpp
)

target_link_libraries(chan_cpp_tests
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(chan_cpp_tests)

target_include_directories(chan_cpp_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
