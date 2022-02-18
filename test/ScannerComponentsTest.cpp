#include <gtest/gtest.h>

#include <string>
#include <vector>
#include "../src/scanner/input_stream/IIndexedStream.h"
#include "../src/scanner/components/NumericConstantsDFSAScanner.h"
#include "../src/scanner/components/CommentsScanner.h"
#include "../src/scanner/components/IdentifierAndKeywordScanner.h"
#include "../src/scanner/components/StringLiteralScanner.h"
#include "../src/scanner/components/OperatorScanner.h"
#include "glog/logging.h"

using namespace std;
using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::scanner::components;

Span span{0, 0};

struct IndexedStringStream: input_stream::IIndexedStream<char> {

    explicit IndexedStringStream(const std::string & string) : _stream(basic_stringstream<char>{string}) {}

    char get() override {
        auto curr = static_cast<char>(_stream.get());
        return curr;
    }

    char peek() override {
        return static_cast<char>(_stream.peek());
    }

    void unget() override {
        _stream.unget();
    }

    uint16_t getRow() override {
        return 0;
    }

    uint16_t getColumn() override {
        return 0;
    }

    string skipToTheEndOfCurrRow() override {
        return "";
    }

private:
    basic_stringstream<char> _stream;
};

TEST(ScannerComponentsTest, NumericConstantsScannerPositiveTest) {
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
        IndexedStringStream stream(input);
        NumericConstantsDFSAScanner scanner{stream};

        auto result = scanner.next();
        EXPECT_TRUE(result);
        LOG(INFO) << "Token result for String " << input.c_str() << ": " << result.getToken().c_str();
    }

}

TEST(ScannerComponentsTest, NumericConstantsScannerNegativeTest) {
    vector<string> inputs{
            "0e0",
            ".x",
            ",",
            "1x0"
    };

    for (const auto &str: inputs) {
        IndexedStringStream stream{str};
        NumericConstantsDFSAScanner scanner{stream};
        auto result = scanner.next();
        EXPECT_FALSE(result);
    }
}

TEST(ScannerComponentsTest, StringLiteralScannerPositiveTest) {
    vector<string> stringLiterals{
            "\' Single Quote String \'",
            R"('Single Quote String \' ')",

            "\"Double Quote String \"",
            R"(" Double Quote String \" \t \t \t \r\n ")",

    };

    for (const string &str: stringLiterals) {
        IndexedStringStream stream{str};
        StringLiteralScanner scanner{stream};
        auto result = scanner.next();
        auto resultToken = dynamic_pointer_cast<TokenStringLiteral>(result.construct(span));
        EXPECT_TRUE(result);
        LOG(INFO)
                << "Token of type " << *resultToken->getType()
                << " result for string " << str
                << ": " << result.getToken()
                << " at " << resultToken->getValue();
    }
}

TEST(ScannerComponentsTest, OperatorScannerTest) {
    vector<string> operators{
            "+", "-", "*", "/", "%", "^", "#", "&", "~", "|", ";", ":",
            ",", ".", "<", ">", "=", "(", ")", "{", "}", "[", "]",
            "<<", ">>", "//", "==", "~=", "<=", ">=", "::", "..", "..."
    };

    for (const string &str: operators) {
        IndexedStringStream stream{str};
        OperatorScanner scanner{stream};
        auto result = scanner.next();
        auto resultTokenType = dynamic_pointer_cast<TokenOperator>(result.construct(span))->getOperator();
        EXPECT_TRUE(result);
        LOG(INFO)
                << "Token result for string " << str
                << ": " << *resultTokenType
                << " and " << result.getToken();
    }
}

TEST(ScannerComponentsTest, IdentifierAndKeywordScannerTest) {
    vector<string> keywords{
            "and", "break", "do", "else", "elseif", "end", "false", "for", "function", "goto",
            "if", "in", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while"
    };

    vector<string> identifiers{
            "aNd", "break_", "_do", "_e_l123se", "Elseif"
    };

    for (const string &str: keywords) {
        IndexedStringStream stream{str};
        IdentifierAndKeywordScanner scanner{stream};
        auto result = scanner.next();
        auto resultTokenType = dynamic_pointer_cast<TokenKeyword>(result.construct(span))->getKeyword();
        EXPECT_TRUE(result);
        LOG(INFO)
                << "Token result for string " << str
                << ": [" << *resultTokenType << " : " << result.getToken() << "]";
    }

    for (const string &str: identifiers) {
        IndexedStringStream stream{str};
        IdentifierAndKeywordScanner scanner{stream};
        auto result = scanner.next();
        auto resultToken = dynamic_pointer_cast<TokenIdentifier>(result.construct(span));
        EXPECT_TRUE(result);
        LOG(INFO)
                << "Token result for string " << str
                << ": [" << resultToken->getValue() << "]";
    }
}

TEST(ScannerComponentsTest, CommentScannerTest){
    string testComment = "-- My first Comment \n"
                         "a = 12";

    IndexedStringStream stream(testComment);
    CommentsScanner scanner(stream);

    auto result = scanner.next();
    EXPECT_TRUE(result);
    auto resultToken = dynamic_pointer_cast<TokenComment>(result.construct(span));
    EXPECT_EQ(resultToken->getValue(), "-- My first Comment ");
}