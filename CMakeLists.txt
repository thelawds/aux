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
add_executable(
        aux
        src/Main.cpp
        src/intermediate_representation/tokens/Token.cpp
        src/scanner/ScanTokenResult.cpp
        src/scanner/components/NumericConstantsDFSAScanner.cpp
        src/scanner/components/IScannerComponent.h src/scanner/components/StringLiteralScanner.cpp src/scanner/components/StringLiteralScanner.h src/scanner/characters/StringLiteral.h src/scanner/components/OperatorScanner.cpp src/scanner/components/OperatorScanner.h src/scanner/components/IdentifierAndKeywordScanner.cpp src/scanner/components/IdentifierAndKeywordScanner.h src/scanner/characters/IdentifierKeyword.h src/scanner/components/CommentsScanner.cpp src/scanner/components/CommentsScanner.h src/scanner/input_stream/PreprocessedFileInputStream.cpp src/scanner/input_stream/PreprocessedFileInputStream.h src/scanner/input_stream/IIndexedStream.h src/scanner/ModularScanner.cpp src/scanner/ModularScanner.h)

add_executable(
        tests
        test/ScannerComponentsTest.cpp
        src/intermediate_representation/tokens/Token.cpp
        src/intermediate_representation/tokens/Token.h
        src/scanner/ScanTokenResult.cpp
        src/scanner/components/NumericConstantsDFSAScanner.cpp
        src/scanner/components/StringLiteralScanner.cpp
        src/scanner/components/StringLiteralScanner.h
        src/scanner/characters/StringLiteral.h
        src/scanner/components/OperatorScanner.cpp
        src/scanner/components/OperatorScanner.h
        src/scanner/components/IdentifierAndKeywordScanner.cpp
        src/scanner/components/IdentifierAndKeywordScanner.h
        src/scanner/characters/IdentifierKeyword.h
        src/scanner/components/CommentsScanner.cpp src/scanner/components/CommentsScanner.h src/scanner/input_stream/PreprocessedFileInputStream.cpp src/scanner/input_stream/PreprocessedFileInputStream.h src/scanner/input_stream/IIndexedStream.h src/scanner/ModularScanner.cpp src/scanner/ModularScanner.h test/ModularScannerTest.cpp)

# Setting Up Testing Framework
target_link_libraries(tests gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)