cmake_minimum_required(VERSION 3.15.0...3.27.0)
project(test_clvlib VERSION 0.1.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

# Main executable
add_executable(test_clvlib)
target_sources(test_clvlib PRIVATE
    cbmi_tests.cpp
    ci_string_tests.cpp
)

if (CMAKE_IMPORT_LIBRARY_SUFFIX)
  add_custom_command(
    TARGET test_clvlib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:test_clvlib> $<TARGET_FILE_DIR:test_clvlib>
    COMMAND_EXPAND_LISTS
  )
endif ()

target_include_directories(test_clvlib PRIVATE
    "${PROJECT_SOURCE_DIR}/../src/"
    )

target_link_libraries(
  test_clvlib PRIVATE
  GTest::gtest_main)


include(GoogleTest)
gtest_discover_tests(test_clvlib)

#add_test(NAME ClvLibUnitTest, COMMAND test_clvlib)


