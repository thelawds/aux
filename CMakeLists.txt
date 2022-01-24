cmake_minimum_required(VERSION 3.21)
project(aux)

set(CMAKE_CXX_STANDARD 20)

# Include tests framework:
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

# Setting Executables:
add_executable(aux src/main.cpp)
add_executable(tests tests/hello_test.cpp)

# Setting Up Testing Framework
target_link_libraries(tests gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)