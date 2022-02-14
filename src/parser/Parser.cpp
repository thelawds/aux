#include "Parser.h"

#include <utility>
#include <unordered_set>
#include <glog/logging.h>
#include "../Exception.h"

using namespace aux::ir::tokens;
using namespace aux::exception;
using namespace aux::ir::ast;
using namespace aux::scanner;
using namespace aux::parser;
using namespace std;

Parser::Parser(shared_ptr<IScanner> scanner) : _scanner(move(scanner)) {}

shared_ptr<Token> Parser::peek() {
    if (_scanner->peek()) {
        return _scanner->peek();
    } else {
        static auto defaultEmptyToken = make_shared<TokenUndefined>(Span(0, 0));
        return defaultEmptyToken;
    }
}

shared_ptr<Token> Parser::next() {
    if (_scanner->peek()) {
        return _scanner->next();
    } else {
        static auto defaultEmptyToken = make_shared<TokenUndefined>(Span(0, 0));
        return defaultEmptyToken;
    }
}

void Parser::skipToken() {
    next();
}

void Parser::checkNextTokenEquals(const string &expected) {
    if (peek()->getRawValue() != expected) {
        throw ParsingException::expressionErrorBuilder()
                .addExpected(expected)
                .withActual(peek()->getRawValue())
                .withSpan(peek()->getSpan())// todo: addCurrentRow
                .build();
    }
}

void Parser::checkNextTokenTypeEquals(const TokenType &expected) {
    if (peek()->getType() != expected) {
        throw ParsingException::expressionErrorBuilder()
                .addExpected(*expected)
                .withActual(*peek()->getType())
                .withSpan(peek()->getSpan())// todo: addCurrentRow
                .build();
    }
}

std::shared_ptr<BaseTree> Parser::parse() {
    return parseExpr();
}

shared_ptr<ListTree> Parser::parseExprList() {
    auto expList = make_shared<ListTree>(ListTree::Type::EXPRESSION_LIST);

    std::shared_ptr<BaseTree> exp = parseExpr();
    while (true) {
        expList->pushBack(exp);
        if (peek()->getRawValue() == *Operator::COMMA) {
            skipToken();
            exp = parseExpr();
            if (!exp) {
                throw ParsingException::expressionErrorBuilder()
                        .addExpected("<, Expression>")
                        .withActual("<, None>")
                        .withSpan(peek()->getSpan())
                        .build();
            }
        } else {
            break;
        }
    }

    return expList;
}

shared_ptr<BaseTree> Parser::parseExpr() {
    LOG(INFO) << "Started Parsing Expression from " + peek()->getRawValue();

    if (peek()->getRawValue() == *Operator::DOT_DOT_DOT) {
        return make_shared<TokenTree>(next());
    }

//    auto functionDef = parseFunctionDef(); //todo
//    if (functionDef) {
//        return functionDef;
//    }

    auto tableConstructor = parseTableConstructor();
    if (tableConstructor) {
        return tableConstructor;
    }

    auto arlExpr = parseArlExpr();
    if (arlExpr) {
        return arlExpr;
    }

    return {nullptr};
}

shared_ptr<PrefixExprTree> Parser::parseFunctionCall() {
    LOG(INFO) << "Started Parsing Function Call from " + peek()->getRawValue();
    auto prefixExpr = parsePrefixExpr();

    auto &suffixes = prefixExpr->suffixes->trees;
    if (suffixes.empty()) {
        return {nullptr};
    }

    auto &lastSuffix = suffixes[suffixes.size() - 1];
    if (dynamic_pointer_cast<FunctionCallSuffixTree>(lastSuffix)) {
        return prefixExpr;
    } else {
        throw ParsingException::expressionErrorBuilder()
                .addExpected("(<List of Arguments>)")
                .withActual(lastSuffix->getPrintValue()) // todo: change?
                        // todo: add Span
                .build();
    }

}

shared_ptr<PrefixExprTree> Parser::parsePrefixExpr() {
    LOG(INFO) << "Started Parsing Prefix Expression from " + peek()->getRawValue();
    auto token = peek();
    shared_ptr<BaseTree> expression{nullptr};
    if (token->getRawValue() == *Operator::LEFT_PARENTHESIS) {
        token = next();
        expression = parseExpr();
        checkNextTokenEquals(*Operator::RIGHT_PARENTHESIS);
        skipToken();
    } else {
        if (token->getType() != ir::tokens::TokenType::IDENTIFIER) {
            return {nullptr};
        }
        token = next();
    }

    auto suffixes = make_shared<ListTree>(ListTree::Type::PE_SUFFIX_LIST);
    while (true) {
        auto peTree = parsePrefixExprSuffix();
        if (peTree) {
            suffixes->pushBack(peTree);
        } else {
            auto funcCall = parseFuncCallSuffix();
            if (funcCall) {
                suffixes->pushBack(funcCall);
            } else {
                break;
            }
        }
    }

    if (expression) {
        return make_shared<PrefixExprTree>(expression, suffixes);
    } else {
        return make_shared<PrefixExprTree>(static_pointer_cast<TokenIdentifier>(token), suffixes);
    }
}

shared_ptr<VariableTree> Parser::parseVariable() {
    LOG(INFO) << "Started Parsing Variable from " + peek()->getRawValue();
    auto token = peek();
    shared_ptr<BaseTree> expression{nullptr};

    if (token->getRawValue() == *Operator::LEFT_PARENTHESIS) {
        expression = parseExpr();
        checkNextTokenEquals(*Operator::RIGHT_PARENTHESIS);
        skipToken();
    } else {
        if (token->getType() != TokenType::IDENTIFIER) {
            return {nullptr};
        }
        token = next();
    }

    auto suffix = make_shared<ListTree>(ListTree::Type::PE_SUFFIX_LIST);
    shared_ptr<ExprSuffixTree> peTree;
    while ((peTree = parsePrefixExprSuffix())) {
        suffix->pushBack(peTree);
    }

    if (expression) {
        return make_shared<VariableTree>(expression, suffix);
    } else {
        return make_shared<VariableTree>(static_pointer_cast<TokenIdentifier>(token), suffix);
    }
}

shared_ptr<ExprSuffixTree> Parser::parsePrefixExprSuffix() {
    LOG(INFO) << "Started Parsing Prefix Expression Suffix from " + peek()->getRawValue();
    if (peek()->getRawValue() == *Operator::LEFT_BRACKET) {
        skipToken();
        auto exp = parseExpr();
        checkNextTokenEquals(*Operator::RIGHT_BRACKET);
        skipToken();
        return make_shared<ExprSuffixTree>(exp);
    } else if (peek()->getRawValue() == *Operator::DOT) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER);
        return make_shared<ExprSuffixTree>(static_pointer_cast<TokenIdentifier>(next()));
    }

    return {nullptr};
}

shared_ptr<FunctionCallSuffixTree> Parser::parseFuncCallSuffix() {
    LOG(INFO) << "Started Parsing Function Call Suffix from " + peek()->getRawValue();

    if (peek()->getType() == TokenType::UNDEFINED) {
        return {nullptr};
    }

    shared_ptr<Token> identifier;
    if (peek()->getRawValue() == *Operator::COLON) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER);
        identifier = next();
    }

    auto args = parseArgs();
    if (args) {
        return make_shared<FunctionCallSuffixTree>(args, static_pointer_cast<TokenIdentifier>(identifier));
    } else {
        return {nullptr};
    }
}

shared_ptr<ArgsTree> Parser::parseArgs() {
    LOG(INFO) << "Started Parsing Args from " + peek()->getRawValue();

    if (peek()->getRawValue() == *Operator::LEFT_PARENTHESIS) {
        skipToken();
        auto result = parseExprList();
        checkNextTokenEquals(*Operator::RIGHT_PARENTHESIS);
        skipToken();
        return make_shared<ArgsTree>(result);
    } else if (peek()->getType() == TokenType::STRING_LITERAL) {
        return make_shared<ArgsTree>(dynamic_pointer_cast<TokenStringLiteral>(next()));
    } else {
        auto tableConstructor = parseTableConstructor();
        if (tableConstructor) {
            return make_shared<ArgsTree>(tableConstructor);
        } else {
            return {nullptr};
        }
    }

}

shared_ptr<ListTree> Parser::parseTableConstructor() {
    LOG(INFO) << "Started Parsing Table Constructor from " + peek()->getRawValue();
    if (peek()->getRawValue() == *Operator::LEFT_CURLY_BRACE) {
        skipToken();

        if (peek()->getRawValue() == *Operator::RIGHT_CURLY_BRACE) {
            skipToken();
            return make_shared<ListTree>(ListTree::Type::TABLE_FIELD_LIST);
        }

        auto result = parseTableFieldList();
        checkNextTokenEquals(*Operator::RIGHT_CURLY_BRACE);
        skipToken();
        return result;
    }

    return {nullptr};
}

shared_ptr<ListTree> Parser::parseTableFieldList() {
    LOG(INFO) << "Started Parsing Table Field List from " + peek()->getRawValue();
    static std::unordered_set<std::string> separators = {*Operator::COMMA, *Operator::SEMI_COLON};

    auto result = make_shared<ListTree>(ListTree::Type::TABLE_FIELD_LIST);
    result->pushBack(parseTableField());
    while (true) {
        if (separators.contains(peek()->getRawValue())) {
            skipToken();
            auto field = parseTableField();
            result->pushBack(field);
        } else {
            break;
        }
    }

    if (separators.contains(peek()->getRawValue())) {
        skipToken();
    }

    return result;
}

shared_ptr<BinTree> Parser::parseTableField() {
    LOG(INFO) << "Started Parsing Table Field from " + peek()->getRawValue();
    if (peek()->getRawValue() == *Operator::LEFT_BRACKET) {
        skipToken();
        auto left = parseExpr();

        checkNextTokenEquals(*Operator::RIGHT_BRACKET);
        skipToken();
        checkNextTokenEquals(*Operator::EQUAL);

        auto opToken = next();
        auto right = parseExpr();

        return make_shared<BinTree>(BinTree::Type::TABLE_FIELD_DECLARATION, left, right, opToken);
    } else if (peek()->getType() == TokenType::IDENTIFIER) {
        shared_ptr<BaseTree> identifier = make_shared<TokenTree>(_scanner->next());
        checkNextTokenEquals(*Operator::EQUAL);
        auto op = next();
        auto right = parseExpr();

        return make_shared<BinTree>(
                BinTree::Type::TABLE_FIELD_DECLARATION,
                identifier, right, op
        );
    } else {
        return make_shared<BinTree>(
                BinTree::Type::TABLE_FIELD_DECLARATION,
                parseExpr(), nullptr, nullptr
        );
    }
}

shared_ptr<BinTree> Parser::parseArlExpr() {
    LOG(INFO) << "Started Parsing Logical Or Term from " + peek()->getRawValue();

    auto result = parseLogicalAndTerm();
    while (peek()->getRawValue() == *Keyword::OR) {
        auto op = next();
        auto right = parseLogicalAndTerm();
        if (right) {
            result = make_shared<BinTree>(
                    BinTree::Type::BINARY_OPERATION,
                    result, right, op
            );
        } else {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("<Expression>")
                    .withActual("<Undefined>")
                    .withSpan(op->getSpan())
                    .build();
        }
    }

    return result;
}


shared_ptr<BinTree> Parser::parseLogicalAndTerm() {
    LOG(INFO) << "Started Parsing Logical And Term from " + peek()->getRawValue();

    auto result = parseRelationalTerm();
    while (peek()->getRawValue() == *Keyword::AND) {
        auto op = next();
        auto right = parseRelationalTerm();
        if (right) {
            result = make_shared<BinTree>(
                    BinTree::Type::BINARY_OPERATION,
                    result, right, op
            );
        } else {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("<Expression>")
                    .withActual("<Undefined>")
                    .withSpan(op->getSpan())
                    .build();
        }
    }

    return result;
}

shared_ptr<BinTree> Parser::parseRelationalTerm() {
    LOG(INFO) << "Started Parsing Relational Term from " + peek()->getRawValue();
    static unordered_set<string> relationalOperators = {
            *Operator::LESS_THAN, *Operator::GREATER_THAN, *Operator::LT_EQUAL,
            *Operator::GT_EQUAL, *Operator::TILDA_EQUAL, *Operator::EQUAL_EQUAL
    };

    auto result = parseBitwiseOrTerm();
    while (relationalOperators.contains(peek()->getRawValue())) {
        auto op = next();
        auto right = parseBitwiseOrTerm();
        if (right) {
            result = make_shared<BinTree>(
                    BinTree::Type::BINARY_OPERATION,
                    result, right, op
            );
        } else {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("<Expression>")
                    .withActual("<Undefined>")
                    .withSpan(op->getSpan())
                    .build();
        }
    }

    return result;
}

shared_ptr<BinTree> Parser::parseBitwiseOrTerm() {
    LOG(INFO) << "Started Parsing Bitwise Or Term from " + peek()->getRawValue();

    auto result = parseBitwiseXorTerm();
    while (peek()->getRawValue() == *Operator::VERTICAL_BAR) {
        auto op = next();
        auto right = parseBitwiseXorTerm();
        if (right) {
            result = make_shared<BinTree>(
                    BinTree::Type::BINARY_OPERATION,
                    result, right, op
            );
        } else {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("<Expression>")
                    .withActual("<Undefined>")
                    .withSpan(op->getSpan())
                    .build();
        }
    }

    return result;
}

shared_ptr<BinTree> Parser::parseBitwiseXorTerm() {
    LOG(INFO) << "Started Parsing Bitwise Xor Term from " + peek()->getRawValue();

    auto result = parseBitwiseAndTerm();
    while (peek()->getRawValue() == *Operator::TILDA) {
        auto op = next();
        auto right = parseBitwiseAndTerm();
        if (right) {
            result = make_shared<BinTree>(
                    BinTree::Type::BINARY_OPERATION,
                    result, right, op
            );
        } else {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("<Expression>")
                    .withActual("<Undefined>")
                    .withSpan(op->getSpan())
                    .build();
        }
    }

    return result;
}
shared_ptr<BinTree> Parser::parseBitwiseAndTerm() {
    LOG(INFO) << "Started Parsing Bitwise And Term from " + peek()->getRawValue();

    auto result = parseShiftedTerm();
    while (peek()->getRawValue() == *Operator::AMPERSAND) {
        auto op = next();
        auto right = parseShiftedTerm();
        if (right) {
            result = make_shared<BinTree>(
                    BinTree::Type::BINARY_OPERATION,
                    result, right, op
            );
        } else {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("<Expression>")
                    .withActual("<Undefined>")
                    .withSpan(op->getSpan())
                    .build();
        }
    }

    return result;
}

std::shared_ptr<BinTree> Parser::parseShiftedTerm(){
    LOG(INFO) << "Started Parsing Shifted Term from " + peek()->getRawValue();
    static unordered_set<string> shiftOperators = {*Operator::LT_LT, *Operator::GT_GT};

    auto result = parseStringConcatenationTerm();
    while (shiftOperators.contains(peek()->getRawValue())) {
        auto op = next();
        auto right = parseStringConcatenationTerm();
        if (right) {
            result = make_shared<BinTree>(
                    BinTree::Type::BINARY_OPERATION,
                    result, right, op
            );
        } else {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("<Expression>")
                    .withActual("<Undefined>")
                    .withSpan(op->getSpan())
                    .build();
        }
    }

    return result;
}

shared_ptr<BinTree> Parser::parseStringConcatenationTerm() {
    LOG(INFO) << "Started Parsing Concatenation Term from " + peek()->getRawValue();

    auto result = parseSummationTerm();
    while (peek()->getRawValue() == *Operator::DOT_DOT) {
        auto op = next();
        auto right = parseSummationTerm();
        if (right) {
            result = make_shared<BinTree>(
                    BinTree::Type::BINARY_OPERATION,
                    result, right, op
            );
        } else {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("<Expression>")
                    .withActual("<Undefined>")
                    .withSpan(op->getSpan())
                    .build();
        }
    }

    return result;
}

shared_ptr<BinTree> Parser::parseSummationTerm() {
    LOG(INFO) << "Started Parsing Summation Term from " + peek()->getRawValue();
    static unordered_set<string> summationOperators = {*Operator::PLUS, *Operator::MINUS};

    auto result = parseProductTerm();
    while (summationOperators.contains(peek()->getRawValue())) {
        auto op = next();
        auto right = parseProductTerm();
        if (right) {
            result = make_shared<BinTree>(
                    BinTree::Type::BINARY_OPERATION,
                    result, right, op
            );
        } else {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("<Expression>")
                    .withActual("<Undefined>")
                    .withSpan(op->getSpan())
                    .build();
        }
    }

    return result;
}

shared_ptr<BinTree> Parser::parseProductTerm() {
    LOG(INFO) << "Started Parsing Product Term from " + peek()->getRawValue();
    static unordered_set<string> productOperators = {
            *Operator::ASTERISK, *Operator::SLASH, *Operator::SLASH_SLASH, *Operator::PERCENT
    };

    auto result = parseUnaryTerm();
    while (productOperators.contains(peek()->getRawValue())) {
        auto op = next();
        auto right = parseUnaryTerm();
        if (right) {
            result = make_shared<BinTree>(
                    BinTree::Type::BINARY_OPERATION,
                    result, right, op
            );
        } else {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("<Expression>")
                    .withActual("<Undefined>")
                    .withSpan(op->getSpan())
                    .build();
        }
    }
    return result;

}

shared_ptr<BinTree> Parser::parseUnaryTerm() {
    LOG(INFO) << "Started Parsing Unary Term from " + peek()->getRawValue();
    static unordered_set<string> unaryOperators = {
            *Keyword::NOT, *Operator::SHARP, *Operator::MINUS, *Operator::TILDA
    };

    if (unaryOperators.contains(peek()->getRawValue())) {
        auto token = next();
        auto exponentTerm = parseExponentTerm();
        if (exponentTerm) {
                return make_shared<BinTree>(
                        BinTree::Type::UNARY_OPERATION,
                        nullptr, exponentTerm, token
                );
        } else {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("<Expression>")
                    .withActual("<Undefined>")
                    .withSpan(token->getSpan())
                    .build();
        }
    } else  {
        return parseExponentTerm();
    }
}

shared_ptr<BinTree> Parser::parseExponentTerm() {
    LOG(INFO) << "Started Parsing Exponent Term from " + peek()->getRawValue();
    auto left = parseTerm();
    if (!left) {
        return {nullptr};
    }

    if (peek()->getRawValue() == *Operator::CARET) {
        auto caret = next();
        auto right = parseExponentTerm();
        return make_shared<BinTree>(
                BinTree::Type::BINARY_OPERATION,
                left, right, caret
        );
    }

    return make_shared<BinTree>(
            BinTree::Type::BINARY_OPERATION,
            left, nullptr, nullptr
    );

}

std::shared_ptr<TermTree> Parser::parseTerm() {
    LOG(INFO) << "Started Parsing Term from " + peek()->getRawValue();
    static auto isTerminal = [](const shared_ptr<Token> &t) -> bool {
        static unordered_set<string> terminalKeywordsAndOperators = {
                *Keyword::NIL, *Keyword::TRUE, *Keyword::FALSE
        };
        static unordered_set<TokenType> terminalTokenTypes = {
                TokenType::NUMERIC_DOUBLE,
                TokenType::NUMERIC_HEX,
                TokenType::NUMERIC_DECIMAL,
                TokenType::STRING_LITERAL
        };
        return terminalKeywordsAndOperators.contains(t->getRawValue())
               || terminalTokenTypes.contains(t->getType());
    };

    if (isTerminal(peek())) {
        return make_shared<TermTree>(next());
    } else {
        auto prefixExpr = parsePrefixExpr();
        if (prefixExpr) {
            return make_shared<TermTree>(prefixExpr);
        } else {
            return {nullptr};
        }
    }


}
