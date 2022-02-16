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

/**
 * Adapters on scanner functions:
 */

shared_ptr<Token> Parser::peek() {
    return _scanner->peek();
}

shared_ptr<Token> Parser::next() {
    return _scanner->next();
}

void Parser::skipToken() {
    next();
}

/**
 * Exception Generation Helping Functions:
 */

#define STATEMENT_ERROR true

void Parser::checkNextTokenEquals(const string &expected, bool isStatement = false) {
    if (peek()->getRawValue() != expected) {
        auto builder = isStatement ? ParsingException::statementErrorBuilder()
                                   : ParsingException::expressionErrorBuilder();
        throw builder
                .addExpected(expected)
                .withActual(peek()->getRawValue())
                .withSpan(peek()->getSpan())// todo: addCurrentRow
                .build();
    }
}

void Parser::checkNextTokenIn(const unordered_set<string> &expected, bool isStatement = false) {
    if (!expected.contains(peek()->getRawValue())) {
        auto builder = isStatement ? ParsingException::statementErrorBuilder()
                                   : ParsingException::expressionErrorBuilder();

        for (const auto &e: expected) {
            builder.addExpected("<" + e + ">");
        }

        throw builder
                .withActual(peek()->getRawValue())
                .withSpan(peek()->getSpan())// todo: addCurrentRow
                .build();
    }
}

void Parser::checkNextTokenTypeEquals(const TokenType &expected, bool isStatement = false) {
    if (peek()->getType() != expected) {
        auto builder = isStatement ? ParsingException::statementErrorBuilder()
                                   : ParsingException::expressionErrorBuilder();
        throw builder
                .addExpected(*expected)
                .withActual(*peek()->getType())
                .withSpan(peek()->getSpan())// todo: addCurrentRow
                .build();
    }
}

/**
 * Actual Parsing implemented using Recursive-Descent Approach
 */

shared_ptr<BaseTree> Parser::parse() {
    try {
        return parseBlock();
    } catch (ParsingException &exception) {
        LOG(FATAL) << exception.what() << "\n Exiting...";
    }
}


shared_ptr<ListTree> Parser::parseBlock() {
    LOG(INFO) << "Started Parsing Block from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    auto result = make_shared<ListTree>(ListTree::Type::STATEMENTS_LIST);

    while (true) {
        auto statement = parseStatement();
        if (!statement) {
            break;
        }
        result->pushBack(statement);
    }

    auto returnStatement = parseReturnStatement();
    if (returnStatement) {
        result->pushBack(returnStatement);
    }

    return result;
}

shared_ptr<BaseTree> Parser::parseStatement() {
    LOG(INFO) << "Started Parsing Statement from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getRawValue() == *Operator::SEMI_COLON || peek()->getRawValue() == *Keyword::BREAK) {
        auto op = next();
        return make_shared<TokenTree>(op);
    } else if (peek()->getRawValue() == *Keyword::GOTO) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER);
        auto identifier = next();
        return make_shared<TokenTree>(TokenTree::Type::GOTO_IDENTIFIER, identifier);
    } else if (peek()->getRawValue() == *Keyword::DO) {
        skipToken();
        auto block = parseBlock();
        checkNextTokenEquals(*Keyword::END), skipToken();
        return block;
    } else if (peek()->getRawValue() == *Keyword::LOCAL) {
        skipToken();
        auto funcDefinition = parseFunctionDefinition();
        if (funcDefinition) {
            return make_shared<BinTree>(
                    BinTree::Type::LOCAL_FUNCTION_DEFINITION,
                    funcDefinition->left, funcDefinition->right,
                    nullptr
            );
        } else {
            auto attributeIdentifierList = parseAttribIdentifierList();
            if (!attributeIdentifierList) {
                throw ParsingException::statementErrorBuilder()
                        .addExpected("<Identifier>")
                        .addExpected("<Function Definition>")
                        .withActual(peek()->getRawValue())
                        .withSpan(peek()->getSpan())
                        .build();
            }
            if (peek()->getRawValue() == *Operator::EQUAL) {
                auto op = next();
                auto expList = parseExprList();
                return make_shared<BinTree>(BinTree::Type::BINARY_OPERATION, attributeIdentifierList, expList, op);
            } else {
                return make_shared<BinTree>(BinTree::Type::BINARY_OPERATION, attributeIdentifierList, nullptr, nullptr);
            }
        }
    }
    auto whileLoop = parseWhileLoop();
    if (whileLoop) {
        return whileLoop;
    }

    auto ifStatement = parseIfStatement();
    if (ifStatement) {
        return ifStatement;
    }

    auto forLoop = parseForLoop();
    if (forLoop) {
        return forLoop;
    }

    auto label = parseLabel();
    if (label) {
        return label;
    }

    auto functionDefinition = parseFunctionDefinition();
    if (functionDefinition) {
        return functionDefinition;
    }

    auto assignmentOrFunctionCall = parseAssignmentOrFunctionCall();
    if (assignmentOrFunctionCall) {
        return assignmentOrFunctionCall;
    }

    return nullptr;
}

shared_ptr<BinTree> Parser::parseAssignmentOrFunctionCall() {
    LOG(INFO) << "Started Parsing Assignment or Function Call from " + peek()->getRawValue() << " at : "
              << peek()->getSpan();

    auto prefixExp = parsePrefixExpr();
    if (!prefixExp) {
        return nullptr;
    }

    auto modifiersSize = prefixExp->suffixes->trees.size();
    auto lastModifier = modifiersSize > 0 ? prefixExp->suffixes->trees[modifiersSize - 1] : nullptr;

    if (lastModifier && dynamic_pointer_cast<FunctionCallSuffixTree>(lastModifier)) {
        return make_shared<BinTree>(BinTree::Type::FUNCTION_CALL, nullptr, prefixExp, nullptr);
    } else {
        checkNextTokenIn({*Operator::COMMA, *Operator::EQUAL});
        if (peek()->getRawValue() == *Operator::COMMA) {
            skipToken();
        }

        auto varList = parseVarList();
        if (!varList) {
            varList = make_shared<ListTree>(ListTree::Type::VARIABLE_LIST);
            varList->pushBack(prefixExp);
        } else {
            varList->trees.insert(varList->trees.begin(), prefixExp);
        }

        auto op = (checkNextTokenEquals(*Operator::EQUAL), next());
        auto exprList = parseExprList();
        if (!exprList) {
            throw ParsingException::statementErrorBuilder()
                    .addExpected("<Expression...>")
                    .withActual(peek()->getRawValue() + " which is not valid <Expression>")
                    .withSpan(peek()->getSpan())
                    .build();
        }
        return make_shared<BinTree>(BinTree::Type::BINARY_OPERATION, varList, exprList, op);
    }
}

shared_ptr<ListTree> Parser::parseIfStatement() {
    LOG(INFO) << "Started Parsing If Statement from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto throwNoExpressionFoundError = [&]() {
        throw ParsingException::statementErrorBuilder()
                .addExpected("<Expression>")
                .withActual(peek()->getRawValue() + ", which is not an expression")
                .withSpan(peek()->getSpan())
                .build();
    };

    if (peek()->getRawValue() != *Keyword::IF) {
        return nullptr;
    }

    skipToken();
    auto result = make_shared<ListTree>(ListTree::Type::IF_THEN_ELSE);
    auto expr = parseExpr();
    if (!expr) {
        throwNoExpressionFoundError();
    }
    checkNextTokenEquals(*Keyword::THEN), skipToken();
    auto block = parseBlock();
    result->pushBack(make_shared<BinTree>(BinTree::Type::IF_THEN, expr, block, nullptr));
    while (peek()->getRawValue() == *Keyword::ELSEIF) {
        skipToken();
        expr = parseExpr();
        if (!expr) {
            throwNoExpressionFoundError();
        }
        checkNextTokenEquals(*Keyword::THEN), skipToken();
        block = parseBlock();
        result->pushBack(make_shared<BinTree>(BinTree::Type::IF_THEN, expr, block, nullptr));
    }

    if (peek()->getRawValue() == *Keyword::ELSE) {
        skipToken();
        block = parseBlock();
        result->pushBack(make_shared<BinTree>(BinTree::Type::ELSE, nullptr, block, nullptr));
    }

    checkNextTokenEquals(*Keyword::END), skipToken();

    return result;
}

shared_ptr<BinTree> Parser::parseWhileLoop() {
    LOG(INFO) << "Started Parsing While Loop from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto throwNoExpressionFoundError = [&]() {
        throw ParsingException::statementErrorBuilder()
                .addExpected("<Expression>")
                .withActual(peek()->getRawValue() + ", which is not an expression")
                .withSpan(peek()->getSpan())
                .build();
    };

    if (peek()->getRawValue() == *Keyword::WHILE) {
        skipToken();
        auto expr = parseExpr();
        if (!expr) {
            throwNoExpressionFoundError();
        }
        checkNextTokenEquals(*Keyword::DO), skipToken();
        auto block = parseBlock();
        checkNextTokenEquals(*Keyword::END), skipToken();
        return make_shared<BinTree>(BinTree::Type::WHILE_LOOP, expr, block, nullptr);
    } else if (peek()->getRawValue() == *Keyword::REPEAT) {
        skipToken();
        auto block = parseBlock();
        checkNextTokenEquals(*Keyword::UNTIL), skipToken();
        auto expr = parseExpr();
        if (!expr) {
            throwNoExpressionFoundError();
        }
        return make_shared<BinTree>(BinTree::Type::REPEAT_UNTIL_LOOP, expr, block, nullptr);
    }

    return nullptr;
}

shared_ptr<BinTree> Parser::parseFunctionDefinition() {
    LOG(INFO) << "Started Parsing Function Definition from " + peek()->getRawValue();

    if (peek()->getRawValue() != *Keyword::FUNCTION) {
        return {nullptr};
    }

    skipToken();
    auto identifier = parseFunctionIdentifier();
    if (!identifier) {
        throw ParsingException::statementErrorBuilder()
                .addExpected("<Identifier>")
                .withActual(
                        peek()->getRawValue() + "of type " + *peek()->getType() + ", which is not valid Identifier"
                )
                .withSpan(peek()->getSpan())
                .build();
    }

    auto funcBody = parseFunctionBody();
    if (!funcBody) {
        throw ParsingException::statementErrorBuilder()
                .addExpected("<(args...)>")
                .withActual(peek()->getRawValue() + ", does not match required <(args...)>")
                .withSpan(peek()->getSpan())
                .build();
    }

    return make_shared<BinTree>(BinTree::Type::FUNCTION_DEFINITION, identifier, funcBody, nullptr);
}

shared_ptr<ForLoopTree> Parser::parseForLoop() {
    LOG(INFO) << "Started Parsing For Loop from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getRawValue() != *Keyword::FOR) {
        return nullptr;
    }

    skipToken();
    auto identifierList = parseIdentifierList();
    if (!identifierList) {
        throw ParsingException::statementErrorBuilder()
                .addExpected("<Identifier>")
                .withActual(peek()->getRawValue())
                .withSpan(peek()->getSpan())
                .build();
    }

    auto op = (checkNextTokenIn({*Operator::EQUAL, *Keyword::IN}), next());
    auto expList = parseExprList();
    if (!expList) {
        throw ParsingException::statementErrorBuilder()
                .addExpected("<Expression*>")
                .withActual(peek()->getRawValue() + ", which is not valid <Expression>")
                .withSpan(peek()->getSpan())
                .build();
    }

    checkNextTokenEquals(*Keyword::DO), skipToken();
    auto block = parseBlock();
    checkNextTokenEquals(*Keyword::END), skipToken();

    return make_shared<ForLoopTree>(identifierList, op, expList, block);
}

shared_ptr<BinTree> Parser::parseReturnStatement() {
    LOG(INFO) << "Started Parsing Return statement from " + peek()->getRawValue() << " at : " << peek()->getSpan();;

    if (peek()->getRawValue() != *Keyword::RETURN) {
        return nullptr;
    }

    auto returnKeyword = next();
    auto exprList = parseExprList();
    if (peek()->getRawValue() == *Operator::SEMI_COLON) {
        skipToken();
    }

    return make_shared<BinTree>(BinTree::Type::BINARY_OPERATION, nullptr, exprList, returnKeyword);
}

shared_ptr<BinTree> Parser::parseAssignment() {
    LOG(INFO) << "Started Parsing Assignment from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto varList = parseVarList();
    if (!varList) {
        return nullptr;
    }

    auto tokEqual = (checkNextTokenEquals(*Operator::EQUAL, STATEMENT_ERROR), next());
    auto expressionList = parseExprList();
    if (!expressionList) {
        throw ParsingException::statementErrorBuilder()
                .addExpected("<Expression> on the right-hand side of an assignment operator")
                .withActual(peek()->getRawValue() + ", which is not and <Expression>")
                .withSpan(peek()->getSpan())
                .build();
    }

    return make_shared<BinTree>(BinTree::Type::BINARY_OPERATION, varList, expressionList, tokEqual);
}

shared_ptr<ListTree> Parser::parseFunctionIdentifier() {
    LOG(INFO) << "Started Parsing Function Identifier from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    if (peek()->getType() != TokenType::IDENTIFIER) {
        return {nullptr};
    }

    auto result = make_shared<ListTree>(ListTree::Type::FUNCTION_IDENTIFIER_SEQUENCE);
    result->pushBack(make_shared<TokenTree>(next()));

    while (peek()->getRawValue() == *Operator::DOT) {
        auto identifier = (skipToken(), checkNextTokenTypeEquals(TokenType::IDENTIFIER, STATEMENT_ERROR), next());
        result->pushBack(make_shared<TokenTree>(TokenTree::Type::DOT_IDENTIFIER, identifier));
    }

    if (peek()->getRawValue() == *Operator::COLON) {
        auto identifier = (skipToken(), checkNextTokenTypeEquals(TokenType::IDENTIFIER, STATEMENT_ERROR), next());
        result->pushBack(make_shared<TokenTree>(TokenTree::Type::COLON_IDENTIFIER, identifier));
    }

    return result;

}

shared_ptr<BinTree> Parser::parseFunctionBody() {
    LOG(INFO) << "Started Parsing Function Body from " + peek()->getRawValue() << " at : " << peek()->getSpan();;

    if (peek()->getRawValue() == *Operator::LEFT_PARENTHESIS) {
        skipToken();

        auto parList = parseParList();

        checkNextTokenEquals(*Operator::RIGHT_PARENTHESIS, STATEMENT_ERROR);
        skipToken();

        auto block = parseBlock();

        checkNextTokenEquals(*Keyword::END, STATEMENT_ERROR);
        skipToken();

        return make_shared<BinTree>(BinTree::Type::FUNCTION_BODY, parList, block, nullptr);
    }

    return {nullptr};
}

shared_ptr<TokenTree> Parser::parseLabel() {
    LOG(INFO) << "Started Parsing Label from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getRawValue() == *Operator::COLON_COLON) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER, STATEMENT_ERROR);

        auto result = make_shared<TokenTree>(TokenTree::Type::LABEL, next());

        checkNextTokenEquals(*Operator::COLON_COLON, STATEMENT_ERROR);
        skipToken();
        return result;
    }

    return {nullptr};
}

shared_ptr<ListTree> Parser::parseAttribIdentifierList() {
    LOG(INFO) << "Started Parsing Attribute Identifiers List from " + peek()->getRawValue() << " at : "
              << peek()->getSpan();
    static unordered_set<string> separators = {*Operator::COMMA, *Operator::DOT_DOT_DOT};

    if (peek()->getType() != TokenType::IDENTIFIER) {
        return {nullptr};
    }

    auto result = make_shared<ListTree>(ListTree::Type::ATTRIBUTE_IDENTIFIER_LIST);
    auto parseAttributedIdentifier = [&]() {
        auto identifier = next();
        auto attribute = parseAttribute();
        if (attribute) {
            result->pushBack(
                    make_shared<BinTree>(
                            BinTree::Type::ATTRIBUTE_IDENTIFIER,
                            make_shared<TokenTree>(identifier), attribute, nullptr
                    )
            );
        } else {
            result->pushBack(make_shared<TokenTree>(identifier));
        }
    };

    parseAttributedIdentifier();
    while (peek()->getRawValue() == *Operator::COMMA) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER, STATEMENT_ERROR);
        parseAttributedIdentifier();
    }

    return result;
}

shared_ptr<BaseTree> Parser::parseParList() {
    LOG(INFO) << "Started Parsing Parameters List from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    if (peek()->getRawValue() == *Operator::DOT_DOT_DOT) {
        return make_shared<TokenTree>(TokenTree::Type::PARAMETER_LIST, next());
    }

    return parseIdentifierList();
}

shared_ptr<ListTree> Parser::parseIdentifierList(bool parseAdditionalDotDotDot) {
    LOG(INFO) << "Started Parsing Identifier List from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getType() != TokenType::IDENTIFIER) {
        return {nullptr};
    }

    auto result = make_shared<ListTree>(ListTree::Type::IDENTIFIER_LIST);
    result->pushBack(make_shared<TokenTree>(next()));

    while (peek()->getRawValue() == *Operator::COMMA) {
        skipToken();

        if (peek()->getType() == TokenType::IDENTIFIER) {
            result->pushBack(make_shared<TokenTree>(next()));
        } else if (parseAdditionalDotDotDot && peek()->getRawValue() == *Operator::DOT_DOT_DOT) {
            result->pushBack(make_shared<TokenTree>(TokenTree::Type::PARAMETER_LIST, next()));
        } else {
            checkNextTokenIn({*Operator::DOT_DOT_DOT, *TokenType::IDENTIFIER}, STATEMENT_ERROR);
        }
    }

    return result;
}

shared_ptr<TokenTree> Parser::parseAttribute() {
    LOG(INFO) << "Started Parsing Attribute from " + peek()->getRawValue();
    if (peek()->getRawValue() == *Operator::LESS_THAN) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER, STATEMENT_ERROR);
        auto result = make_shared<TokenTree>(TokenTree::Type::ATTRIBUTE, next());
        checkNextTokenEquals(*Operator::GREATER_THAN, STATEMENT_ERROR);
        skipToken();
        return result;
    }
    return {nullptr};
}

shared_ptr<ListTree> Parser::parseExprList() {
    auto expList = make_shared<ListTree>(ListTree::Type::EXPRESSION_LIST);

    shared_ptr<BaseTree> exp = parseExpr();
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
    LOG(INFO) << "Started Parsing Expression from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getRawValue() == *Operator::DOT_DOT_DOT) {
        return make_shared<TokenTree>(next());
    } else if (peek()->getRawValue() == *Keyword::FUNCTION) {
        skipToken();
        auto functionDef = parseFunctionBody();
        if (functionDef) {
            return functionDef;
        }
    } else if (peek()->getRawValue() == *Operator::LEFT_CURLY_BRACE) {
        auto tableConstructor = parseTableConstructor();
        if (tableConstructor) {
            return tableConstructor;
        }
    } else {
        auto arlExpr = parseArlExpr();
        if (arlExpr) {
            return arlExpr;
        }
    }

    return {nullptr};
}

shared_ptr<PrefixExprTree> Parser::parseFunctionCall() {
    LOG(INFO) << "Started Parsing Function Call from " + peek()->getRawValue() << " at : " << peek()->getSpan();
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
    LOG(INFO) << "Started Parsing Prefix Expression from " + peek()->getRawValue() << " at : " << peek()->getSpan();
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

shared_ptr<ListTree> Parser::parseVarList() {
    LOG(INFO) << "Started Parsing Variable List from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto var = parseVariable();
    if (!var) {
        return nullptr;
    }

    auto result = make_shared<ListTree>(ListTree::Type::VARIABLE_LIST);
    result->pushBack(var);
    while (peek()->getRawValue() == *Operator::COMMA) {
        var = (skipToken(), parseVariable());
        if (!var) {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("Another <VariableReference> after ,")
                    .withActual(peek()->getRawValue() + " , which is not valid <VariableReference>")
                    .withSpan(peek()->getSpan())
                    .build();
        }
        result->pushBack(var);
    }

    return result;
}

shared_ptr<VariableTree> Parser::parseVariable() {
    LOG(INFO) << "Started Parsing Variable from " + peek()->getRawValue() << " at : " << peek()->getSpan();
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
    LOG(INFO) << "Started Parsing Prefix Expression Suffix from " + peek()->getRawValue() << " at : "
              << peek()->getSpan();
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
    LOG(INFO) << "Started Parsing Function Call Suffix from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getType() == TokenType::EOF_OR_UNDEFINED) {
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
    LOG(INFO) << "Started Parsing Args from " + peek()->getRawValue() << " at : " << peek()->getSpan();

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
    LOG(INFO) << "Started Parsing Table Constructor from " + peek()->getRawValue() << " at : " << peek()->getSpan();
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
    LOG(INFO) << "Started Parsing Table Field List from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    static unordered_set<string> separators = {*Operator::COMMA, *Operator::SEMI_COLON};

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
    LOG(INFO) << "Started Parsing Table Field from " + peek()->getRawValue() << " at : " << peek()->getSpan();
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
    LOG(INFO) << "Started Parsing Logical Or Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();

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
    LOG(INFO) << "Started Parsing Logical And Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();

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
    LOG(INFO) << "Started Parsing Relational Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
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
    LOG(INFO) << "Started Parsing Bitwise Or Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();

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
    LOG(INFO) << "Started Parsing Bitwise Xor Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();

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
    LOG(INFO) << "Started Parsing Bitwise And Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();

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

shared_ptr<BinTree> Parser::parseShiftedTerm() {
    LOG(INFO) << "Started Parsing Shifted Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
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
    LOG(INFO) << "Started Parsing Concatenation Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();

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
    LOG(INFO) << "Started Parsing Summation Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
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
    LOG(INFO) << "Started Parsing Product Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
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
    LOG(INFO) << "Started Parsing Unary Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
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
    } else {
        return parseExponentTerm();
    }
}

shared_ptr<BinTree> Parser::parseExponentTerm() {
    LOG(INFO) << "Started Parsing Exponent Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
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

shared_ptr<TermTree> Parser::parseTerm() {
    LOG(INFO) << "Started Parsing Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
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
