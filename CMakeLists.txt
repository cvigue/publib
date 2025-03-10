cmake_minimum_required(VERSION 3.15.0...3.27.0)
project(clvlib VERSION 0.1.0)

enable_testing()
# Suppress warnings related to DartConfiguration.tcl
set_tests_properties(${clvlib} PROPERTIES LABELS "NoDart")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# if(MINGW)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
#     add_compile_definitions(_WIN32_WINNT=0x0601)
# endif()

# Add compiler-specific flags for Linux-y stuff - add -Wextra later
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Adding -Wno-sign-conversion to suppress warnings from the asio library
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wconversion -Wno-sign-conversion")
endif()

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_definitions(_WIN32_WINNT=0x0601)
endif()

# Set the OpenSSL root directory for Windows builds
if(WIN32)
    set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
    set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
    set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.lib)
    set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.lib)
endif()

# Set the build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Enable the generation of compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Store the original CXX flags
set(ORIGINAL_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Suppress warnings based on compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w")
endif()

# Add external dependencies with suppressed warnings
add_subdirectory(extern/googletest)

# Restore the original CXX flags
set(CMAKE_CXX_FLAGS "${ORIGINAL_CXX_FLAGS}")

add_subdirectory(tests)

# option(ENABLE_DOXYGEN "Build code documentation with Doxygen")