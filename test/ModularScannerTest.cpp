//
// Created by miserable on 31.01.2022.
//

#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include <vector>

#include "../src/scanner/ModularScanner.h"
#include "../src/scanner/input_stream/PreprocessedFileInputStream.h"

using namespace std;
using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::scanner::components;
using namespace aux::scanner::input_stream;

TEST(ModularScannerTest, TestPositiveInputCases){
    PreprocessedFileInputStream fis{"../test/resources/test_cases/ScannerTest.lua"};
    ModularScanner scanner{fis};

    while (true) {
        auto token = scanner.next();
        if (!token) {
            break;
        }

        switch (token->getType()) {
            case TokenType::IDENTIFIER:
                CommonOutputStream
                        << "Identifier " <<  dynamic_pointer_cast<TokenIdentifier>(token)->getValue()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::KEYWORD:
                CommonOutputStream
                        << "Keyword " << dynamic_pointer_cast<TokenKeyword>(token)->getKeyword()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::STRING_LITERAL:
                CommonOutputStream
                        << "String Literal " << dynamic_pointer_cast<TokenStringLiteral>(token)->getValue()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::OPERATOR:
                CommonOutputStream
                        << "Operator " << dynamic_pointer_cast<TokenOperator>(token)->getOperator()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::COMMENT:
                CommonOutputStream
                        << "Comment " << dynamic_pointer_cast<TokenComment>(token)->getValue()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::NUMERIC_DECIMAL:
                CommonOutputStream
                        << "Decimal " << dynamic_pointer_cast<TokenDecimal>(token)->getValue()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::NUMERIC_HEX:
                CommonOutputStream
                        << "Hex " << dynamic_pointer_cast<TokenHex>(token)->getValue()
                        << " of " <<dynamic_pointer_cast<TokenHex>(token)->getRawValue()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::NUMERIC_DOUBLE:
                CommonOutputStream
                        << "Double " << dynamic_pointer_cast<TokenDouble>(token)->getValue()
                        << " of " <<dynamic_pointer_cast<TokenDouble>(token)->getRawValue()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::UNDEFINED:
                CommonOutputStream
                        << "Undefined"
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
        }

    }
}