#include <gtest/gtest.h>

#include <string>
#include <vector>
#include "../src/scanner/components/NumericConstantsDFSAScanner.h"
#include "../src/scanner/components/IdentifierAndKeywordScanner.h"
#include "../src/scanner/components/StringLiteralScanner.h"
#include "../src/scanner/components/OperatorScanner.h"

using namespace std;
using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::scanner::components;


TEST(ScannerTest, NumericConstantsScannerPositiveTest) {
    vector<char> delimiters = {
            '+', '-', '*', '/', '(', ')', '[', ' ', '\t'
    };
    vector<string> inputs{
            "0", "3", "345", "0xff", "0xBEBADA",
            "3.0", "3.1416", "314.16e-2", "0.31416E1", "34e1",
            "0x0.1E", "0xA23p-4", "0X1.921FB54442D18P+1"
    };

    for (string input: inputs) {
        input += delimiters[random() % delimiters.size()];
        stringstream stream(input);
        NumericConstantsDFSAScanner scanner{stream};

        Span span{0, 0};
        auto result = scanner.next(span);
        EXPECT_TRUE(result);
        printf(
                "Token result for String (%s): (%s)\n", input.c_str(), result.getToken().c_str()
        );
    }

}

TEST(ScannerTest, NumericConstantsScannerNegativeTest) {
    vector<string> inputs{
            "0e0",
            ".x",
            ",",
            "1x0"
    };

    for (const auto &str: inputs) {
        stringstream stream{str};
        NumericConstantsDFSAScanner scanner{stream};
        auto result = scanner.next({0, 0});
        EXPECT_FALSE(result);
    }
}

TEST(ScannerTest, StringLiteralScannerPositiveTest) {
    Span span{0, 0};
    vector<string> stringLiterals{
            "\' Single Quote String \'",
            "\'Single Quote String \\\' \'",

            "\"Double Quote String \"",
            "\" Double Quote String \\\" \\t \\t \\t \\r\\n \"",

    };

    for (const string &str: stringLiterals) {
        stringstream stream{str};
        StringLiteralScanner scanner{stream};
        auto result = scanner.next(span);
        auto resultToken = dynamic_pointer_cast<TokenStringLiteral>(result.construct(span));
        EXPECT_TRUE(result);
        printf(
                "Token of type %d result for string (%s): (%s) at (%s)\n",
                resultToken->getType(), str.c_str(), result.getToken().c_str(), resultToken->getValue().c_str()
        );
    }
}

TEST(ScannerTest, OperatorScannerTest) {
    Span span{0, 0};
    vector<string> stringLiterals{
            "+", "-", "*", "/", "%", "^", "#", "&", "~", "|", ";", ":",
            ",", ".", "<", ">", "=", "(", ")", "{", "}", "[", "]",
            "<<", ">>", "//", "==", "~=", "<=", ">=", "::", "..", "..."
    };

    for (const string &str: stringLiterals) {
        stringstream stream{str};
        OperatorScanner scanner{stream};
        auto result = scanner.next(span);
        auto resultTokenType = dynamic_pointer_cast<TokenOperator>(result.construct(span))->getValue();
        EXPECT_TRUE(result);
        printf("Token result for string %s: %d and %s\n", str.c_str(), resultTokenType, result.getToken().c_str());
    }
}

TEST(ScannerTest, IdentifierAndKeywordScannerTest) {
    Span span{0, 0};
    vector<string> keywords{
            "and", "break", "do", "else", "elseif", "end", "false", "for", "function", "goto",
            "if", "in", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while"
    };

    vector<string> identifiers{
            "aNd", "break_", "_do", "_e_l123se", "Elseif"
    };

    for (const string &str: keywords) {
        stringstream stream{str};
        IdentifierAndKeywordScanner scanner{stream};
        auto result = scanner.next(span);
        auto resultTokenType = dynamic_pointer_cast<TokenKeyword>(result.construct(span))->getKeyword();
        EXPECT_TRUE(result);
        printf("Token result for string (%s): [%d : %s]\n", str.c_str(), resultTokenType, result.getToken().c_str());
    }

    for (const string &str: identifiers) {
        stringstream stream{str};
        IdentifierAndKeywordScanner scanner{stream};
        auto result = scanner.next(span);
        auto resultToken = dynamic_pointer_cast<TokenIdentifier>(result.construct(span));
        EXPECT_TRUE(result);
        printf("Token result for string (%s): [%s]\n", str.c_str(), resultToken->getValue().c_str());
    }
}