//
// Created by miserable on 28.01.2022.
//

#include "Token.h"

#include <utility>

using namespace aux::ir::tokens;

const std::unordered_map<CommonStringType, Keyword> keywords  {
        {toCommonStringType("and"),      Keyword::AND},
        {toCommonStringType("break"),    Keyword::BREAK},
        {toCommonStringType("do"),       Keyword::DO},
        {toCommonStringType("else"),     Keyword::ELSE},
        {toCommonStringType("elseif"),   Keyword::ELSEIF},
        {toCommonStringType("end"),      Keyword::END},
        {toCommonStringType("false"),    Keyword::FALSE},
        {toCommonStringType("for"),      Keyword::FOR},
        {toCommonStringType("function"), Keyword::FUNCTION},
        {toCommonStringType("goto"),     Keyword::GOTO},
        {toCommonStringType("if"),       Keyword::IF},
        {toCommonStringType("in"),       Keyword::IN},
        {toCommonStringType("local"),    Keyword::LOCAL},
        {toCommonStringType("nil"),      Keyword::NIL},
        {toCommonStringType("not"),      Keyword::NOT},
        {toCommonStringType("or"),       Keyword::OR},
        {toCommonStringType("repeat"),   Keyword::REPEAT},
        {toCommonStringType("return"),   Keyword::RETURN},
        {toCommonStringType("then"),     Keyword::THEN},
        {toCommonStringType("true"),     Keyword::TRUE},
        {toCommonStringType("until"),    Keyword::UNTIL},
        {toCommonStringType("while"),    Keyword::WHILE}
};

const std::unordered_map<CommonStringType, Operator> operators {
        {toCommonStringType("+"), Operator::PLUS},
        {toCommonStringType("-"), Operator::MINUS},
        {toCommonStringType("*"), Operator::ASTERISK},
        {toCommonStringType("/"), Operator::SLASH},
        {toCommonStringType("%"), Operator::PERCENT},
        {toCommonStringType("^"), Operator::CARET},
        {toCommonStringType("#"), Operator::SHARP},
        {toCommonStringType("&"), Operator::AMPERSAND},
        {toCommonStringType("~"), Operator::TILDA},
        {toCommonStringType("|"), Operator::VERTICAL_BAR},
        {toCommonStringType("<<"), Operator::LT_LT},
        {toCommonStringType(">>"), Operator::GT_GT},
        {toCommonStringType("//"), Operator::SLASH_SLASH},
        {toCommonStringType("=="), Operator::EQUAL_EQUAL},
        {toCommonStringType("~="), Operator::TILDA_EQUAL},
        {toCommonStringType("<="), Operator::LT_EQUAL},
        {toCommonStringType(">="), Operator::GT_EQUAL},
        {toCommonStringType("<"), Operator::LESS_THAN},
        {toCommonStringType(">"), Operator::GREATER_THAN},
        {toCommonStringType("="), Operator::EQUAL},
        {toCommonStringType("("), Operator::LEFT_PARENTHESIS},
        {toCommonStringType(")"), Operator::RIGHT_PARENTHESIS},
        {toCommonStringType("["), Operator::LEFT_BRACKET},
        {toCommonStringType("]"), Operator::RIGHT_BRACKET},
        {toCommonStringType("{"), Operator::LEFT_CURLY_BRACE},
        {toCommonStringType("}"), Operator::RIGHT_CURLY_BRACE},
        {toCommonStringType("::"), Operator::COLON_COLON},
        {toCommonStringType(";"), Operator::SEMI_COLON},
        {toCommonStringType(":"), Operator::COLON},
        {toCommonStringType(","), Operator::COMMA},
        {toCommonStringType("."), Operator::DOT},
        {toCommonStringType(".."), Operator::DOT_DOT},
        {toCommonStringType("..."), Operator::DOT_DOT_DOT}
};

bool aux::ir::tokens::isKeyword(const CommonStringType &str){
    return keywords.contains(str);
}

Span Token::getSpan() const {
    return this->_span;
}

TokenType TokenIdentifier::getType() const {
    return TokenType::IDENTIFIER;
}

TokenIdentifier::TokenIdentifier(CommonStringType value, const Span &span) : Token(span), _value(std::move(value)) {}

const CommonStringType &TokenIdentifier::getValue() const {
    return this->_value;
}

std::string TokenIdentifier::getRawValue() const {
    return getValue();
}

TokenKeyword::TokenKeyword(const CommonStringType &value, const Span &span) : Token(span), _keyword(keywords.at(value)) {}

TokenType TokenKeyword::getType() const {
    return TokenType::KEYWORD;
}

const Keyword &TokenKeyword::getKeyword() {
    return this->_keyword;
}

std::string TokenKeyword::getRawValue() const {
    return *this->_keyword;
}

TokenStringLiteral::TokenStringLiteral(CommonStringType value, const Span &span) : Token(span), _value(std::move(value)) {}

TokenType TokenStringLiteral::getType() const {
    return TokenType::STRING_LITERAL;
}

const CommonStringType &TokenStringLiteral::getValue() const {
    return _value;
}

std::string TokenStringLiteral::getRawValue() const {
    return getValue();
}

TokenType TokenOperator::getType() const {
    return TokenType::OPERATOR;
}

TokenOperator::TokenOperator(const CommonStringType &value, const Span &span)
        : Token(span), _value(operators.at(value)) {}

TokenOperator::TokenOperator(const Operator &value, const Span &span)
        : Token(span), _value(value) {}

const Operator &TokenOperator::getOperator() const {
    return _value;
}

std::string TokenOperator::getRawValue() const {
    return *this->getOperator();
}

TokenEofOrUndefined::TokenEofOrUndefined(const Span &span) : Token(span) {}

TokenType TokenEofOrUndefined::getType() const {
    return TokenType::EOF_OR_UNDEFINED;
}

std::string TokenEofOrUndefined::getRawValue() const {
    return "EOF or Undefined Token";
}


TokenComment::TokenComment(CommonStringType value, const Span &span)
        : Token(span), _value(std::move(value)) {}

TokenType TokenComment::getType() const {
    return TokenType::COMMENT;
}

const CommonStringType &TokenComment::getValue() const {
    return _value;
}

std::string TokenComment::getRawValue() const {
    return getValue();
}

