cmake_minimum_required(VERSION 3.15.0...3.27.0)
project(clvlib VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_compile_definitions(_WIN32_WINNT=0x0601)

add_subdirectory(extern/googletest)
add_subdirectory(tests)

# option(ENABLE_DOXYGEN "Build code documentation with Doxygen")

