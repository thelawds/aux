//
// Created by miserable on 10.02.2022.
//

#include "Parser.h"

#include <utility>

using namespace aux::ir::tokens;
using namespace aux::ir::ast;
using namespace aux::scanner;
using namespace aux::parser;
using namespace std;

Parser::Parser(shared_ptr<IScanner> scanner) : _scanner(move(scanner)) {}

std::shared_ptr<TreeNode> Parser::parse() {
    return parseExp();
}

shared_ptr<TreeNode> Parser::parseExp() {
    return parsePrefixExp();
}

shared_ptr<TreeNode> Parser::parseFunctionCall() {
    auto prefixExp = parsePrefixExp();
    if (prefixExp->suffixes.at(prefixExp->suffixes.size() - 1) == ir::ast::FUNC_CALL) {
        return prefixExp;
    } else {
        auto funcCall = parseFuncCallSuffix();
        prefixExp->addPeOrFuncCallSuffix(funcCall);
        return prefixExp;
    }

    return {nullptr};
}

shared_ptr<PrefixExprTree> Parser::parsePrefixExp() {
    auto token = _scanner->next();
    shared_ptr<PrefixExprTree> resultTree;
    if (token->getType() == TokenType::IDENTIFIER) {
        resultTree = make_shared<PrefixExprTree>(static_pointer_cast<TokenIdentifier>(token));
    } else if ((token->getRawValue()) == *Operator::LEFT_PARENTHESIS) {
        auto exp = parseExp();
        token = _scanner->next();
        if ((token->getRawValue()) == *Operator::RIGHT_PARENTHESIS) {
            resultTree = make_shared<PrefixExprTree>(exp);
        }
    }

    if (resultTree) {

        while (true) {
            auto peTree = parsePeSuffix();
            if (peTree) {
                resultTree->addPeOrFuncCallSuffix(peTree);
            } else {
                auto funcCall = parseFuncCallSuffix();
                if (funcCall) {
                    resultTree->addPeOrFuncCallSuffix(funcCall);
                } else {
                    break;
                }
            }
        }

        return resultTree;
    }


    return {nullptr};
}

shared_ptr<VarTree> Parser::parseVar() {
    auto token = _scanner->next();
    shared_ptr<VarTree> resultTree;
    if (token->getType() == TokenType::IDENTIFIER) {
        resultTree = make_shared<VarTree>(static_pointer_cast<TokenIdentifier>(token));
    } else if ((token->getRawValue()) == *Operator::LEFT_PARENTHESIS) {
        auto exp = parseExp();
        token = _scanner->next();
        if ((token->getRawValue()) == *Operator::RIGHT_PARENTHESIS) {
            resultTree = make_shared<VarTree>(exp);
        }
    }

    if (resultTree) {
        shared_ptr<PeSuffixTree> peTree;
        while (peTree = parsePeSuffix(), peTree) {
            resultTree->addPeSuffix(peTree);
        }
        return resultTree;
    }

    return {nullptr};
}

shared_ptr<PeSuffixTree> Parser::parsePeSuffix() {
    auto curr = _scanner->peek();
    if (curr && (curr->getRawValue()) == *Operator::LEFT_BRACKET) {
        curr = _scanner->next();
        auto exp = parseExp();
        curr = _scanner->next();
        if ((curr->getRawValue()) == *Operator::RIGHT_BRACKET) {
            return make_shared<PeSuffixTree>(exp);
        }
    } else if (curr && (curr->getRawValue()) == *Operator::DOT) {
        _scanner->next(); // skip '.'
        auto id = _scanner->next();
        if (id->getType() == TokenType::IDENTIFIER) {
            return make_shared<PeSuffixTree>(static_pointer_cast<TokenIdentifier>(id));
        }
    }

    return {nullptr};
}

shared_ptr<FunctionCallTree> Parser::parseFuncCallSuffix() {
    shared_ptr<Token> identifier;
    if (_scanner->peek() && (_scanner->peek()->getRawValue()) == *Operator::COLON) {
        _scanner->next();
        identifier = _scanner->next();
        if (identifier->getType() != TokenType::IDENTIFIER) {
            return {nullptr};
        }
    }
    auto args = parseArgs();

    if (args) {
        return make_shared<FunctionCallTree>(args, static_pointer_cast<TokenIdentifier>(identifier));
    }
    return {nullptr};
}

shared_ptr<ArgsTree> Parser::parseArgs() {
    auto curr = _scanner->next();
    if (curr && ((curr->getRawValue()) == *Operator::LEFT_PARENTHESIS)) {
        curr = _scanner->next();
        if (curr && ((curr->getRawValue()) == *Operator::RIGHT_PARENTHESIS)) {
            return make_shared<ArgsTree>();
        }
    }

    return {nullptr};
}
