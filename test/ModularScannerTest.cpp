//
// Created by miserable on 31.01.2022.
//

#include "../src/util/Defines.h"

#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include <vector>

#include "../src/scanner/ModularScanner.h"
#include "../src/scanner/input_stream/PreprocessedFileInputStream.h"
#include "glog/logging.h"

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
                LOG(INFO)
                        << "Identifier " <<  dynamic_pointer_cast<TokenIdentifier>(token)->getValue()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::KEYWORD:
                LOG(INFO)
                        << "Keyword " << *dynamic_pointer_cast<TokenKeyword>(token)->getKeyword()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::STRING_LITERAL:
                LOG(INFO)
                        << "String Literal " << dynamic_pointer_cast<TokenStringLiteral>(token)->getValue()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::OPERATOR:
                LOG(INFO)
                        << "Operator " << *dynamic_pointer_cast<TokenOperator>(token)->getOperator()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::COMMENT:
                LOG(INFO)
                        << "Comment " << dynamic_pointer_cast<TokenComment>(token)->getValue()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::NUMERIC_DECIMAL:
                LOG(INFO)
                        << "Decimal " << dynamic_pointer_cast<TokenDecimal>(token)->getValue()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::NUMERIC_HEX:
                LOG(INFO)
                        << "Hex " << dynamic_pointer_cast<TokenHex>(token)->getValue()
                        << " of " <<dynamic_pointer_cast<TokenHex>(token)->getRawValue()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::NUMERIC_DOUBLE:
                LOG(INFO)
                        << "Double " << dynamic_pointer_cast<TokenDouble>(token)->getValue()
                        << " of " <<dynamic_pointer_cast<TokenDouble>(token)->getRawValue()
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
            case TokenType::UNDEFINED:
                LOG(INFO)
                        << "Undefined"
                        << " at (" << token->getSpan().row << ", " << token->getSpan().column << ")"
                        << endl;
                break;
        }

    }
}