//
// Created by miserable on 28.01.2022.
//

#include "Token.h"

#include <utility>

using namespace aux::ir::tokens;

Span Token::getSpan() const {
    return this->_span;
}

TokenType TokenIdentifier::getType() const {
    return TokenType::IDENTIFIER;
}

TokenIdentifier::TokenIdentifier(std::string value, const Span &span) : Token(span), _value(std::move(value)) {}

const std::string &TokenIdentifier::getValue() const {
    return this->_value;
}

TokenKeyword::TokenKeyword(const std::string &value, const Span &span) : Token(span), _keyword(keywords.at(value)) {}

TokenType TokenKeyword::getType() const {
    return TokenType::KEYWORD;
}

const Keyword &TokenKeyword::getKeyword() {
    return this->_keyword;
}


TokenStringLiteral::TokenStringLiteral(std::string value, const Span &span) : Token(span), _value(std::move(value)) {}

TokenType TokenStringLiteral::getType() const {
    return TokenType::STRING_LITERAL;
}

std::string TokenStringLiteral::getValue() const {
    return _value;
}

TokenOperator::TokenOperator(const Span &span) : Token(span) {}

TokenType TokenOperator::getType() const {
    return TokenType::OPERATOR;
}

TokenDelimiter::TokenDelimiter(const Span &span) : Token(span) {}

TokenType TokenDelimiter::getType() const {
    return TokenType::DELIMITER;
}

TokenUndefined::TokenUndefined(const Span &span) : Token(span) {}

TokenType TokenUndefined::getType() const {
    return TokenType::UNDEFINED;
}
