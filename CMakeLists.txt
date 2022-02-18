cmake_minimum_required(VERSION 3.21)
project(aux VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

find_package(glog 0.6.0 REQUIRED)
find_package(gflags REQUIRED)

set(OGDF_CONFIG_SEARCH_PATH PATH "./.libs/ogdf/")
find_package(OGDF)

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
add_executable(
        aux
        src/Main.cpp
        src/intermediate_representation/Token.cpp
        src/scanner/fsa/State.h
        src/scanner/ScanTokenResult.cpp
        src/scanner/components/NumericConstantsDFSAScanner.cpp
        src/scanner/components/StringLiteralScanner.cpp
        src/scanner/components/OperatorScanner.cpp
        src/scanner/components/IdentifierAndKeywordScanner.cpp
        src/scanner/components/CommentsScanner.cpp
        src/scanner/input_stream/PreprocessedFileInputStream.cpp
        src/scanner/ModularScanner.cpp
        src/parser/Parser.cpp
        src/parser/Parser.h
        src/intermediate_representation/Tree.h
        src/exception/Exception.h
)

add_executable(
        tests
        test/ScannerComponentsTest.cpp
        src/intermediate_representation/Token.cpp
        src/scanner/fsa/State.h
        src/scanner/ScanTokenResult.cpp
        src/scanner/components/NumericConstantsDFSAScanner.cpp
        src/scanner/components/StringLiteralScanner.cpp
        src/scanner/components/OperatorScanner.cpp
        src/scanner/components/IdentifierAndKeywordScanner.cpp
        src/scanner/components/CommentsScanner.cpp
        src/scanner/input_stream/PreprocessedFileInputStream.cpp
        src/scanner/ModularScanner.cpp
        test/ModularScannerTest.cpp
        test/ParserTest.cpp
        src/parser/Parser.cpp
        src/parser/Parser.h
        src/intermediate_representation/Tree.h
)

# Setting Up Frameworks
# Testing
target_include_directories(tests PRIVATE ${OGDF_INCLUDE_DIRS})
target_link_libraries(tests gtest_main glog::glog OGDF)
include(GoogleTest)
gtest_discover_tests(tests)

# Application
target_link_libraries(aux glog::glog gflags)
