project(test_clvlib VERSION 0.1.0)

# Main executable
add_executable(test_clvlib)
target_sources(test_clvlib PRIVATE
    cbmi_tests.cpp
    ci_string_tests.cpp
    intrinsic_type_tests.cpp
    intrusive_tests.cpp
    runtime_dispatcher_tests.cpp
    tuple_help_tests.cpp
)

target_include_directories(test_clvlib PRIVATE
    "${PROJECT_SOURCE_DIR}/../src/"
    )

target_link_libraries(
  test_clvlib PRIVATE
  GTest::gtest_main)


include(GoogleTest)
gtest_discover_tests(test_clvlib)



