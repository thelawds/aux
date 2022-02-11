cmake_minimum_required(VERSION 3.21)
project(aux VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

find_package(glog 0.6.0 REQUIRED)
find_package(gflags REQUIRED)

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
        src/intermediate_representation/tokens/Token.cpp
        src/scanner/ScanTokenResult.cpp
        src/scanner/components/NumericConstantsDFSAScanner.cpp
        src/scanner/components/StringLiteralScanner.cpp
        src/scanner/components/OperatorScanner.cpp
        src/scanner/components/IdentifierAndKeywordScanner.cpp
        src/scanner/components/CommentsScanner.cpp
        src/scanner/input_stream/PreprocessedFileInputStream.cpp
        src/scanner/ModularScanner.cpp
        src/intermediate_representation/ast/ArithmeticLogicalExpressionTree.h src/parser/Parser.cpp src/parser/Parser.h src/intermediate_representation/ast/TreeNode.h src/intermediate_representation/ast/ArgsTree.h src/intermediate_representation/ast/FunctionCallTree.h src/intermediate_representation/ast/PeSuffixTree.h src/intermediate_representation/ast/PrefixExprTree.h)

add_executable(
        tests
        test/ScannerComponentsTest.cpp
        src/intermediate_representation/tokens/Token.cpp
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
        src/intermediate_representation/ast/TreeNode.h
        src/intermediate_representation/ast/ArgsTree.h
        src/intermediate_representation/ast/VarTree.h
        src/intermediate_representation/ast/PeSuffixTree.h)


# Setting Up Frameworks
# Testing
target_link_libraries(tests gtest_main glog::glog)
include(GoogleTest)
gtest_discover_tests(tests)

# Application
target_link_libraries(aux glog::glog gflags)
