#include "Parser.h"

#include <utility>
#include <unordered_set>
#include <glog/logging.h>
#include "exception/Exception.h"

using namespace aux::ir::tokens;
using namespace aux::exception;
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

#define STATEMENT_ERROR         true
#define ALLOW_TRAILING_COMMA    true

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
            builder.addExpected("< " + e + " >");
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

void Parser::throwInvalidBinaryExpressionException(const shared_ptr<TokenOperator> &op) {
    throw ParsingException::expressionErrorBuilder()
            .addExpected("<Expression> " + op->getRawValue() + " <Expression>")
            .withActual("<Expression> " + op->getRawValue() + " <Empty>")
            .withSpan(op->getSpan())
            .build();
}

void Parser::throwInvalidBinaryExpressionException(const shared_ptr<TokenKeyword> &keyword) {
    throw ParsingException::expressionErrorBuilder()
            .addExpected("<Expression> " + keyword->getRawValue() + " <Expression>")
            .withActual("<Expression> " + keyword->getRawValue() + " <Empty>")
            .withSpan(keyword->getSpan())
            .build();
}


/**
 * Actual Parsing implemented using Recursive-Descent Approach
 */

shared_ptr<aux::ir::syntax_tree::ProgramTree> Parser::parse() {
    try {
        return parseBlock();
    } catch (const ParsingException &exception) {
        LOG(FATAL) << exception.what() << "\n Exiting...";
    }
}


shared_ptr<aux::ir::syntax_tree::ProgramTree> Parser::parseBlock() {
    LOG(INFO) << "Started Parsing Block from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto result = make_shared<aux::ir::syntax_tree::ProgramTree>();

    while (true) {
        auto statement = parseStatement();
        if (!statement) {
            break;
        }

        if (!dynamic_pointer_cast<aux::ir::syntax_tree::EmptyStatementTree>(statement)) {
            result->addStatement(statement);
        }
    }

    if (auto returnStatement = parseReturnStatement()) {
        result->addStatement(returnStatement);
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::StatementTree> Parser::parseStatement() {
    LOG(INFO) << "Started Parsing Statement from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getRawValue() == *Operator::SEMI_COLON) {
        skipToken();
        return make_shared<aux::ir::syntax_tree::EmptyStatementTree>();
    } else if (peek()->getRawValue() == *Keyword::BREAK) {
        return make_shared<aux::ir::syntax_tree::KeywordStatementTree>(
                static_pointer_cast<TokenKeyword>(next())
        );
    } else if (peek()->getRawValue() == *Keyword::GOTO) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER);
        return make_shared<aux::ir::syntax_tree::GotoStatementTree>(
                static_pointer_cast<TokenIdentifier>(next())
        );
    } else if (peek()->getRawValue() == *Keyword::DO) {
        skipToken();
        auto block = parseBlock();
        checkNextTokenEquals(*Keyword::END);
        skipToken();
        return block;
    } else if (peek()->getRawValue() == *Keyword::LOCAL) {
        skipToken();
        auto funcDefinition = parseFunctionDefinition();
        if (funcDefinition) {
            funcDefinition->makeLocal();
            return funcDefinition;
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
                skipToken();
                auto expList = parseExprList();
                return make_shared<aux::ir::syntax_tree::AttributeIdentifierAssignmentTree>(
                        attributeIdentifierList, expList
                );
            } else {
                return attributeIdentifierList;
            }
        }
    }

    if (auto whileLoop = parseWhileLoop()) {
        return whileLoop;
    }

    if (auto ifStatement = parseIfStatement()) {
        return ifStatement;
    }

    if (auto forLoop = parseForLoop()) {
        return forLoop;
    }

    if (auto label = parseLabel()) {
        return label;
    }

    if (auto functionDefinition = parseFunctionDefinition()) {
        return functionDefinition;
    }

    if (auto assignmentOrFunctionCall = parseAssignmentOrFunctionCall()) {
        return assignmentOrFunctionCall;
    }

    return nullptr;
}

shared_ptr<aux::ir::syntax_tree::StatementTree> Parser::parseAssignmentOrFunctionCall() {
    LOG(INFO) << "Started Parsing Assignment or Function Call from " + peek()->getRawValue() << " at : "
              << peek()->getSpan();

    auto prefixExp = parsePrefixExpr();
    if (!prefixExp) {
        return nullptr;
    }

    auto modifiersSize = prefixExp->peSuffixTrees.size();
    auto lastModifier = modifiersSize > 0 ? prefixExp->peSuffixTrees[modifiersSize - 1] : nullptr;

    if (lastModifier && dynamic_pointer_cast<aux::ir::syntax_tree::FunctionCallSuffixTree>(lastModifier)) {
        return prefixExp;
    } else {
        checkNextTokenIn({*Operator::COMMA, *Operator::EQUAL});
        if (peek()->getRawValue() == *Operator::COMMA) {
            skipToken();
        }


        auto varList = parseVarList();
        if (varList) {
            varList->variableTrees.insert(
                    varList->variableTrees.begin(),
                    make_shared<aux::ir::syntax_tree::VariableTree>(
                            dynamic_pointer_cast<aux::ir::syntax_tree::IdentifierTermTree>(prefixExp->expressionOrIdentifier),
                            prefixExp->peSuffixTrees
                    )
            );
        } else {
            varList = make_shared<aux::ir::syntax_tree::VariableListTree>();
            varList->addVariable(
                    make_shared<aux::ir::syntax_tree::VariableTree>(
                            dynamic_pointer_cast<aux::ir::syntax_tree::IdentifierTermTree>(prefixExp->expressionOrIdentifier),
                            prefixExp->peSuffixTrees
                    )
            );
        }

        checkNextTokenEquals(*Operator::EQUAL);
        skipToken();
        auto exprList = parseExprList();
        if (!exprList) {
            throw ParsingException::statementErrorBuilder()
                    .addExpected("<Expression...>")
                    .withActual(peek()->getRawValue() + " which is not valid <Expression>")
                    .withSpan(peek()->getSpan())
                    .build();
        }

        return make_shared<aux::ir::syntax_tree::AssignmentTree>(
                varList, exprList
        );
    }
}

shared_ptr<aux::ir::syntax_tree::IfThenElseStatementTree> Parser::parseIfStatement() {
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

    auto result = make_shared<aux::ir::syntax_tree::IfThenElseStatementTree>();

    skipToken();
    auto expression = parseExpr();
    if (!expression) {
        throwNoExpressionFoundError();
    }
    checkNextTokenEquals(*Keyword::THEN);
    skipToken();
    auto body = parseBlock();
    result->addExpressionAndBody(expression, body);

    while (peek()->getRawValue() == *Keyword::ELSEIF) {
        skipToken();
        expression = parseExpr();
        if (!expression) {
            throwNoExpressionFoundError();
        }
        checkNextTokenEquals(*Keyword::THEN);
        skipToken();
        body = parseBlock();
        result->addExpressionAndBody(expression, body);
    }

    if (peek()->getRawValue() == *Keyword::ELSE) {
        skipToken();
        body = parseBlock();
        result->addElseBody(body);
    }

    checkNextTokenEquals(*Keyword::END);
    skipToken();

    return result;
}

shared_ptr<aux::ir::syntax_tree::WhileLoopTree> Parser::parseWhileLoop() {
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
        auto expression = parseExpr();
        if (!expression) {
            throwNoExpressionFoundError();
        }
        checkNextTokenEquals(*Keyword::DO);
        skipToken();
        auto body = parseBlock();
        checkNextTokenEquals(*Keyword::END);
        skipToken();
        return make_shared<aux::ir::syntax_tree::WhileLoopTree>(expression, body);
    } else if (peek()->getRawValue() == *Keyword::REPEAT) {
        skipToken();
        auto block = parseBlock();
        checkNextTokenEquals(*Keyword::UNTIL);
        skipToken();
        auto expr = parseExpr();
        if (!expr) {
            throwNoExpressionFoundError();
        }

        return make_shared<aux::ir::syntax_tree::RepeatUntilTree>(expr, block);
    }

    return nullptr;
}

shared_ptr<aux::ir::syntax_tree::FunctionDefinitionTree> Parser::parseFunctionDefinition() {
    LOG(INFO) << "Started Parsing Function Definition from " + peek()->getRawValue();

    if (peek()->getRawValue() != *Keyword::FUNCTION) {
        return {nullptr};
    }

    skipToken();
    auto identifier = parseFunctionIdentifier();
    if (!identifier) {
        throw ParsingException::statementErrorBuilder()
                .addExpected("<Function Identifier>")
                .withActual(
                        peek()->getRawValue() + " of type " + *peek()->getType()
                        + ", which is not valid Function Identifier"
                )
                .withSpan(peek()->getSpan())
                .build();
    }

    auto functionBody = parseFunctionBody();
    if (!functionBody) {
        throw ParsingException::statementErrorBuilder()
                .addExpected("<(args...) list of expressions END>")
                .withActual(peek()->getRawValue() + ", does not match required <(args...) list of expressions END>")
                .withSpan(peek()->getSpan())
                .build();
    }

    return make_shared<aux::ir::syntax_tree::FunctionDefinitionTree>(identifier, functionBody);
}

shared_ptr<aux::ir::syntax_tree::ForLoopStatementTree> Parser::parseForLoop() {
    LOG(INFO) << "Started Parsing For Loop from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getRawValue() != *Keyword::FOR) {
        return nullptr;
    }

    skipToken();
    auto identifierList = parseIdentifierList();
    if (!identifierList) {
        throw ParsingException::statementErrorBuilder()
                .addExpected("List of <Identifier>")
                .withActual(peek()->getRawValue())
                .withSpan(peek()->getSpan())
                .build();
    }

    checkNextTokenIn({*Operator::EQUAL, *Keyword::IN});
    skipToken();

    auto expressionList = parseExprList();
    if (!expressionList) {
        throw ParsingException::statementErrorBuilder()
                .addExpected("List of <Expression>")
                .withActual(peek()->getRawValue() + ", which is not valid List of <Expression>")
                .withSpan(peek()->getSpan())
                .build();
    }

    checkNextTokenEquals(*Keyword::DO);
    skipToken();

    auto block = parseBlock();

    checkNextTokenEquals(*Keyword::END);
    skipToken();

    return make_shared<aux::ir::syntax_tree::ForLoopStatementTree>(
            identifierList, expressionList, block
    );
}

shared_ptr<aux::ir::syntax_tree::ReturnStatementTree> Parser::parseReturnStatement() {
    LOG(INFO) << "Started Parsing Return statement from " + peek()->getRawValue() << " at : " << peek()->getSpan();;

    if (peek()->getRawValue() != *Keyword::RETURN) {
        return nullptr;
    }

    skipToken();
    auto exprList = parseExprList();
    if (peek()->getRawValue() == *Operator::SEMI_COLON) {
        skipToken();
    }

    return make_shared<aux::ir::syntax_tree::ReturnStatementTree>(exprList);
}

shared_ptr<aux::ir::syntax_tree::FunctionIdentifierTree> Parser::parseFunctionIdentifier() {
    LOG(INFO) << "Started Parsing Function Identifier from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    if (peek()->getType() != TokenType::IDENTIFIER) {
        return nullptr;
    }

    auto result = make_shared<aux::ir::syntax_tree::FunctionIdentifierTree>(
            static_pointer_cast<TokenIdentifier>(next())
    );

    while (peek()->getRawValue() == *Operator::DOT) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER, STATEMENT_ERROR);
        auto identifier = static_pointer_cast<TokenIdentifier>(next());
        result->addSecondaryIdentifier(identifier);
    }

    if (peek()->getRawValue() == *Operator::COLON) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER, STATEMENT_ERROR);
        result->setColonIdentifier(
                static_pointer_cast<TokenIdentifier>(next())
        );
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::FunctionBodyTree> Parser::parseFunctionBody() {
    LOG(INFO) << "Started Parsing Function Body from " + peek()->getRawValue() << " at : " << peek()->getSpan();;

    if (peek()->getRawValue() == *Operator::LEFT_PARENTHESIS) {
        skipToken();

        auto parList = parseParList();

        checkNextTokenEquals(*Operator::RIGHT_PARENTHESIS, STATEMENT_ERROR);
        skipToken();

        auto body = parseBlock();

        checkNextTokenEquals(*Keyword::END, STATEMENT_ERROR);
        skipToken();

        return make_shared<aux::ir::syntax_tree::FunctionBodyTree>(parList, body);
    }

    return nullptr;
}

shared_ptr<aux::ir::syntax_tree::LabelTree> Parser::parseLabel() {
    LOG(INFO) << "Started Parsing Label from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getRawValue() == *Operator::COLON_COLON) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER, STATEMENT_ERROR);

        auto result = make_shared<aux::ir::syntax_tree::LabelTree>(
                static_pointer_cast<TokenIdentifier>(next())
        );

        checkNextTokenEquals(*Operator::COLON_COLON, STATEMENT_ERROR);
        skipToken();
        return result;
    }

    return nullptr;
}

shared_ptr<aux::ir::syntax_tree::AttributeIdentifierListTree> Parser::parseAttribIdentifierList() {
    LOG(INFO) << "Parsing Attribute Identifiers List from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    static unordered_set<string> separators = {*Operator::COMMA, *Operator::DOT_DOT_DOT};

    if (peek()->getType() != TokenType::IDENTIFIER) {
        return nullptr;
    }

    shared_ptr<aux::ir::syntax_tree::AttributeIdentifierListTree> result;
    auto parseAttributedIdentifier = [&]() {
        auto identifier = next();
        result->addAttributeIdentifier(
                make_shared<aux::ir::syntax_tree::IdentifierTermTree>(
                        static_pointer_cast<TokenIdentifier>(identifier)
                ),
                parseAttribute()
        );
    };

    parseAttributedIdentifier();
    while (peek()->getRawValue() == *Operator::COMMA) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER, STATEMENT_ERROR);
        parseAttributedIdentifier();
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::ParameterListTree> Parser::parseParList() {
    LOG(INFO) << "Started Parsing Parameters List from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto identifierList = parseIdentifierList(ALLOW_TRAILING_COMMA);

    if (peek()->getRawValue() == *Operator::DOT_DOT_DOT) {
        skipToken();
        return make_shared<aux::ir::syntax_tree::ParameterListTree>(identifierList, true);
    }

    return make_shared<aux::ir::syntax_tree::ParameterListTree>(identifierList, false);
}

shared_ptr<aux::ir::syntax_tree::IdentifierTermListTree> Parser::parseIdentifierList(bool allowTrailingComma) {
    LOG(INFO) << "Started Parsing Identifier List from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getType() != TokenType::IDENTIFIER) {
        return nullptr;
    }

    auto result = make_shared<aux::ir::syntax_tree::IdentifierTermListTree>();
    result->addIdentifier(
            make_shared<aux::ir::syntax_tree::IdentifierTermTree>(
                    static_pointer_cast<TokenIdentifier>(next())
            )
    );

    while (peek()->getRawValue() == *Operator::COMMA) {
        skipToken();
        if (allowTrailingComma && peek()->getType() != ir::tokens::TokenType::IDENTIFIER) {
            break;
        }

        checkNextTokenTypeEquals(TokenType::IDENTIFIER, STATEMENT_ERROR);
        result->addIdentifier(
                make_shared<aux::ir::syntax_tree::IdentifierTermTree>(
                        static_pointer_cast<TokenIdentifier>(next())
                )
        );
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::AttributeTree> Parser::parseAttribute() {
    LOG(INFO) << "Started Parsing Attribute from " + peek()->getRawValue();
    if (peek()->getRawValue() == *Operator::LESS_THAN) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER, STATEMENT_ERROR);
        auto result = make_shared<aux::ir::syntax_tree::AttributeTree>(
                static_pointer_cast<TokenIdentifier>(next())
        );
        checkNextTokenEquals(*Operator::GREATER_THAN, STATEMENT_ERROR), skipToken();
        return result;
    }
    return nullptr;
}

shared_ptr<aux::ir::syntax_tree::ExpressionListTree> Parser::parseExprList() {
    auto expList = make_shared<aux::ir::syntax_tree::ExpressionListTree>();

    shared_ptr<aux::ir::syntax_tree::ExpressionTree> exp = parseExpr();
    while (true) {
        expList->addExpression(exp);
        if (peek()->getRawValue() == *Operator::COMMA) {
            auto comma = static_pointer_cast<TokenOperator>(next());
            exp = parseExpr();
            if (!exp) {
                throwInvalidBinaryExpressionException(comma);
            }
        } else {
            break;
        }
    }

    return expList;
}

shared_ptr<aux::ir::syntax_tree::ExpressionTree> Parser::parseExpr() {
    LOG(INFO) << "Started Parsing Expression from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getRawValue() == *Operator::DOT_DOT_DOT) {
        return make_shared<aux::ir::syntax_tree::KeywordTermTree>(
                static_pointer_cast<TokenKeyword>(next())
        );
    } else if (peek()->getRawValue() == *Keyword::FUNCTION) {
        skipToken();
        if (auto functionDef = parseFunctionBody()) {
            return make_shared<aux::ir::syntax_tree::FunctionDefinitionTree>(functionDef);
        }
    } else if (peek()->getRawValue() == *Operator::LEFT_CURLY_BRACE) {
        return parseTableConstructor();
    } else {
        return parseArlExpr();
    }

    return nullptr;
}

shared_ptr<aux::ir::syntax_tree::PrefixExpressionTermTree> Parser::parsePrefixExpr() {
    LOG(INFO) << "Started Parsing Prefix Expression from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    shared_ptr<aux::ir::syntax_tree::ExpressionTree> expression;

    if (peek()->getRawValue() == *Operator::LEFT_PARENTHESIS) {
        skipToken();
        expression = parseExpr();
        checkNextTokenEquals(*Operator::RIGHT_PARENTHESIS), skipToken();
    } else if (peek()->getType() == ir::tokens::TokenType::IDENTIFIER) {
        expression = make_shared<aux::ir::syntax_tree::IdentifierTermTree>(
                static_pointer_cast<TokenIdentifier>(next())
        );
    } else {
        return nullptr;
    }

    auto result = make_shared<aux::ir::syntax_tree::PrefixExpressionTermTree>(expression);
    while (true) {
        auto peTree = parsePrefixExprSuffix();
        if (peTree) {
            result->addPrefixExpressionSuffix(peTree);
        } else {
            auto funcCall = parseFuncCallSuffix();
            if (funcCall) {
                result->addPrefixExpressionSuffix(funcCall);
            } else {
                break;
            }
        }
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::VariableListTree> Parser::parseVarList() {
    LOG(INFO) << "Started Parsing Variable List from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto var = parseVariable();
    if (!var) {
        return nullptr;
    }

    shared_ptr<aux::ir::syntax_tree::VariableListTree> result = make_shared<aux::ir::syntax_tree::VariableListTree>();
    result->addVariable(var);
    while (peek()->getRawValue() == *Operator::COMMA) {
        skipToken();
        var = parseVariable();
        if (!var) {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected("Another <VariableReference> after ,")
                    .withActual(peek()->getRawValue() + " , which is not valid <VariableReference>")
                    .withSpan(peek()->getSpan())
                    .build();
        }
        result->addVariable(var);
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::VariableTree> Parser::parseVariable() {
    LOG(INFO) << "Started Parsing Variable from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getType() != TokenType::IDENTIFIER) {
        return nullptr;
    }

    auto identifier = make_shared<aux::ir::syntax_tree::IdentifierTermTree>(
            dynamic_pointer_cast<TokenIdentifier>(next())
    );

    auto result = make_shared<aux::ir::syntax_tree::VariableTree>(identifier);
    shared_ptr<aux::ir::syntax_tree::PrefixExpressionSuffixTree> peTree;
    while ((peTree = parsePrefixExprSuffix())) {
        result->addPrefixExprSuffix(peTree);
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::PrefixExpressionSuffixTree> Parser::parsePrefixExprSuffix() {
    LOG(INFO) << "Started Parsing Prefix Expression Suffix from " + peek()->getRawValue() << " at : "
              << peek()->getSpan();

    if (peek()->getRawValue() == *Operator::LEFT_BRACKET) {
        skipToken();
        auto exp = parseExpr();
        checkNextTokenEquals(*Operator::RIGHT_BRACKET);
        skipToken();
        return make_shared<aux::ir::syntax_tree::TableFieldAccessSuffixTree>(exp);
    } else if (peek()->getRawValue() == *Operator::DOT) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER);

        return make_shared<aux::ir::syntax_tree::StructAccessSuffixTree>(
                static_pointer_cast<TokenIdentifier>(next())
        );
    }

    return nullptr;
}

shared_ptr<aux::ir::syntax_tree::FunctionCallSuffixTree> Parser::parseFuncCallSuffix() {
    LOG(INFO) << "Started Parsing Function Call Suffix from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getType() == TokenType::EOF_OR_UNDEFINED) {
        return nullptr;
    }

    shared_ptr<aux::ir::syntax_tree::TokenIdentifier> identifier{nullptr};
    if (peek()->getRawValue() == *Operator::COLON) {
        skipToken();
        checkNextTokenTypeEquals(TokenType::IDENTIFIER);
        identifier = static_pointer_cast<TokenIdentifier>(next());
    }

    auto args = parseArgs();
    if (args) {
        return make_shared<aux::ir::syntax_tree::FunctionCallSuffixTree>(identifier, args);
    } else {
        return nullptr;
    }
}

shared_ptr<aux::ir::syntax_tree::ArgumentsTree> Parser::parseArgs() {
    LOG(INFO) << "Started Parsing Args from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    if (peek()->getRawValue() == *Operator::LEFT_PARENTHESIS) {
        skipToken();
        auto result = parseExprList();
        checkNextTokenEquals(*Operator::RIGHT_PARENTHESIS), skipToken();
        return result;
    } else if (peek()->getType() == TokenType::STRING_LITERAL) {
        return make_shared<aux::ir::syntax_tree::StringLiteralTermTree>(
                static_pointer_cast<TokenStringLiteral>(next())
        );
    } else {
        return parseTableConstructor();
    }

}

shared_ptr<aux::ir::syntax_tree::TableConstructorTermTree> Parser::parseTableConstructor() {
    LOG(INFO) << "Started Parsing Table Constructor from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    if (peek()->getRawValue() == *Operator::LEFT_CURLY_BRACE) {
        skipToken();

        if (peek()->getRawValue() == *Operator::RIGHT_CURLY_BRACE) {
            skipToken();
            return make_shared<aux::ir::syntax_tree::TableConstructorTermTree>();
        }

        auto result = parseTableFieldList();
        checkNextTokenEquals(*Operator::RIGHT_CURLY_BRACE), skipToken();
        return result;
    }

    return nullptr;
}

shared_ptr<aux::ir::syntax_tree::TableConstructorTermTree> Parser::parseTableFieldList() {
    LOG(INFO) << "Started Parsing Table Field List from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    static unordered_set<string> separators = {*Operator::COMMA, *Operator::SEMI_COLON};

    shared_ptr<aux::ir::syntax_tree::TableConstructorTermTree> result = make_shared<aux::ir::syntax_tree::TableConstructorTermTree>();

    result->addField(parseTableField());
    while (true) {
        if (separators.contains(peek()->getRawValue())) {
            skipToken();
            if (peek()->getRawValue() == *Operator::RIGHT_CURLY_BRACE) {
                break;
            }

            auto field = parseTableField();
            result->addField(field);
        } else {
            break;
        }
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::TableFieldTermTree> Parser::parseTableField() {
    LOG(INFO) << "Started Parsing Table Field from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    if (peek()->getRawValue() == *Operator::LEFT_BRACKET) {
        skipToken();

        auto left = parseExpr();

        checkNextTokenEquals(*Operator::RIGHT_BRACKET), skipToken();
        checkNextTokenEquals(*Operator::EQUAL), skipToken();

        auto right = parseExpr();

        return make_shared<aux::ir::syntax_tree::TableFieldTermTree>(left, right);
    } else if (peek()->getType() == TokenType::IDENTIFIER) {
        auto left = make_shared<aux::ir::syntax_tree::IdentifierTermTree>(
                static_pointer_cast<TokenIdentifier>(next())
        );

        checkNextTokenEquals(*Operator::EQUAL), skipToken();
        auto right = parseExpr();

        return make_shared<aux::ir::syntax_tree::TableFieldTermTree>(left, right);
    } else {
        return make_shared<aux::ir::syntax_tree::TableFieldTermTree>(parseExpr());
    }
}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseArlExpr() {
    LOG(INFO) << "Started Parsing Logical Or Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto result = parseLogicalAndTerm();
    while (peek()->getRawValue() == *Keyword::OR) {
        auto op = static_pointer_cast<TokenKeyword>(next());
        auto right = parseRelationalTerm();
        if (right) {
            result = make_shared<aux::ir::syntax_tree::LogicalExpressionTermTree>(result, right, op);
        } else {
            throwInvalidBinaryExpressionException(op);
        }
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseLogicalAndTerm() {
    LOG(INFO) << "Started Parsing Logical And Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto result = parseRelationalTerm();
    while (peek()->getRawValue() == *Keyword::AND) {
        auto op = static_pointer_cast<TokenKeyword>(next());
        auto right = parseRelationalTerm();
        if (right) {
            result = make_shared<aux::ir::syntax_tree::LogicalExpressionTermTree>(result, right, op);
        } else {
            throwInvalidBinaryExpressionException(op);
        }
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseRelationalTerm() {
    LOG(INFO) << "Started Parsing Relational Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    static unordered_set<string> relationalOperators = {
            *Operator::LESS_THAN, *Operator::GREATER_THAN, *Operator::LT_EQUAL,
            *Operator::GT_EQUAL, *Operator::TILDA_EQUAL, *Operator::EQUAL_EQUAL
    };

    auto result = parseBitwiseOrTerm();
    while (relationalOperators.contains(peek()->getRawValue())) {
        auto op = static_pointer_cast<TokenOperator>(next());
        auto right = parseBitwiseOrTerm();
        if (right) {
            result = make_shared<aux::ir::syntax_tree::BinaryExpressionTermTree>(result, right, op);
        } else {
            throwInvalidBinaryExpressionException(op);
        }
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseBitwiseOrTerm() {
    LOG(INFO) << "Started Parsing Bitwise Or Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto result = parseBitwiseXorTerm();
    while (peek()->getRawValue() == *Operator::VERTICAL_BAR) {
        auto op = static_pointer_cast<TokenOperator>(next());
        auto right = parseBitwiseXorTerm();
        if (right) {
            result = make_shared<aux::ir::syntax_tree::BinaryExpressionTermTree>(result, right, op);
        } else {
            throwInvalidBinaryExpressionException(op);
        }
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseBitwiseXorTerm() {
    LOG(INFO) << "Started Parsing Bitwise Xor Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto result = parseBitwiseAndTerm();
    while (peek()->getRawValue() == *Operator::TILDA) {
        auto op = static_pointer_cast<TokenOperator>(next());
        auto right = parseBitwiseAndTerm();
        if (right) {
            result = make_shared<aux::ir::syntax_tree::BinaryExpressionTermTree>(result, right, op);
        } else {
            throwInvalidBinaryExpressionException(op);
        }
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseBitwiseAndTerm() {
    LOG(INFO) << "Started Parsing Bitwise And Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto result = parseShiftedTerm();
    while (peek()->getRawValue() == *Operator::AMPERSAND) {
        auto op = static_pointer_cast<TokenOperator>(next());
        auto right = parseShiftedTerm();
        if (right) {
            result = make_shared<aux::ir::syntax_tree::BinaryExpressionTermTree>(result, right, op);
        } else {
            throwInvalidBinaryExpressionException(op);
        }
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseShiftedTerm() {
    LOG(INFO) << "Started Parsing Shifted Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    static unordered_set<string> shiftOperators = {*Operator::LT_LT, *Operator::GT_GT};

    auto result = parseStringConcatenationTerm();
    while (shiftOperators.contains(peek()->getRawValue())) {
        auto op = static_pointer_cast<TokenOperator>(next());
        auto right = parseStringConcatenationTerm();
        if (right) {
            result = make_shared<aux::ir::syntax_tree::BinaryExpressionTermTree>(result, right, op);
        } else {
            throwInvalidBinaryExpressionException(op);
        }
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseStringConcatenationTerm() {
    LOG(INFO) << "Started Parsing Concatenation Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();

    auto result = parseSummationTerm();
    while (peek()->getRawValue() == *Operator::DOT_DOT) {
        auto op = static_pointer_cast<TokenOperator>(next());
        auto right = parseSummationTerm();
        if (right) {
            result = make_shared<aux::ir::syntax_tree::BinaryExpressionTermTree>(result, right, op);
        } else {
            throwInvalidBinaryExpressionException(op);
        }
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseSummationTerm() {
    LOG(INFO) << "Started Parsing Summation Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    static unordered_set<string> summationOperators = {*Operator::PLUS, *Operator::MINUS};

    auto result = parseProductTerm();
    while (summationOperators.contains(peek()->getRawValue())) {
        auto op = static_pointer_cast<TokenOperator>(next());
        auto right = parseProductTerm();
        if (right) {
            result = make_shared<aux::ir::syntax_tree::BinaryExpressionTermTree>(result, right, op);
        } else {
            throwInvalidBinaryExpressionException(op);
        }
    }

    return result;
}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseProductTerm() {
    LOG(INFO) << "Started Parsing Product Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    static unordered_set<string> productOperators = {
            *Operator::ASTERISK, *Operator::SLASH, *Operator::SLASH_SLASH, *Operator::PERCENT
    };

    shared_ptr<aux::ir::syntax_tree::TermTree> result = parseUnaryTerm();
    while (productOperators.contains(peek()->getRawValue())) {
        auto op = static_pointer_cast<TokenOperator>(next());
        auto right = parseUnaryTerm();
        if (right) {
            result = make_shared<aux::ir::syntax_tree::BinaryExpressionTermTree>(result, right, op);
        } else {
            throwInvalidBinaryExpressionException(op);
        }
    }
    return result;

}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseUnaryTerm() {
    LOG(INFO) << "Started Parsing Unary Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    static unordered_set<string> unaryOperators = {
            *Keyword::NOT, *Operator::SHARP, *Operator::MINUS, *Operator::TILDA
    };

    if (unaryOperators.contains(peek()->getRawValue())) {
        auto token = next();
        auto exponentTerm = parseExponentTerm();
        if (exponentTerm) {
            if (token->getType() == TokenType::KEYWORD) {
                return make_shared<aux::ir::syntax_tree::UnaryTermTree>(
                        static_pointer_cast<TokenKeyword>(token),
                        exponentTerm
                );
            } else {
                return make_shared<aux::ir::syntax_tree::UnaryTermTree>(
                        static_pointer_cast<TokenOperator>(token),
                        exponentTerm
                );
            }
        } else {
            throw ParsingException::expressionErrorBuilder()
                    .addExpected(token->getRawValue() + " <Expression>")
                    .withActual(token->getRawValue() + " <Empty>")
                    .withSpan(token->getSpan())
                    .build();
        }
    } else {
        return parseExponentTerm();
    }
}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseExponentTerm() {
    LOG(INFO) << "Started Parsing Exponent Term from " + peek()->getRawValue() << " at : " << peek()->getSpan();
    auto left = parseTerm();
    if (!left) {
        return nullptr;
    }

    if (peek()->getRawValue() == *Operator::CARET) {
        skipToken();
        auto right = parseExponentTerm();
        return make_shared<aux::ir::syntax_tree::ExponentTermTree>(left, right);
    } else {
        return left;
    }

}

shared_ptr<aux::ir::syntax_tree::TermTree> Parser::parseTerm() {
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
        switch (peek()->getType()) {
            case TokenType::KEYWORD:
                return make_shared<aux::ir::syntax_tree::KeywordTermTree>(
                        static_pointer_cast<TokenKeyword>(next())
                );
            case TokenType::NUMERIC_DECIMAL:
                return make_shared<aux::ir::syntax_tree::IntegerTermTree>(
                        static_pointer_cast<TokenDecimal>(next())
                );
            case TokenType::NUMERIC_HEX:
                return make_shared<aux::ir::syntax_tree::IntegerTermTree>(
                        static_pointer_cast<TokenHex>(next())
                );
            case TokenType::NUMERIC_DOUBLE:
                return make_shared<aux::ir::syntax_tree::DoubleTermTree>(
                        static_pointer_cast<TokenDouble>(next())
                );
            case TokenType::STRING_LITERAL:
                return make_shared<aux::ir::syntax_tree::StringLiteralTermTree>(
                        static_pointer_cast<TokenStringLiteral>(next())
                );
            default:
                break;
        }
    }

    if (auto prefixExpr = parsePrefixExpr()) {
        return prefixExpr;
    }

    return nullptr;
}
