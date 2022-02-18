//
// Created by miserable on 28.01.2022.
//

#include "Token.h"

#include <utility>

using namespace aux::ir::tokens;

const std::unordered_map<std::string, Keyword> keywords  {
        {("and"),      Keyword::AND},
        {("break"),    Keyword::BREAK},
        {("do"),       Keyword::DO},
        {("else"),     Keyword::ELSE},
        {("elseif"),   Keyword::ELSEIF},
        {("end"),      Keyword::END},
        {("false"),    Keyword::FALSE},
        {("for"),      Keyword::FOR},
        {("function"), Keyword::FUNCTION},
        {("goto"),     Keyword::GOTO},
        {("if"),       Keyword::IF},
        {("in"),       Keyword::IN},
        {("local"),    Keyword::LOCAL},
        {("nil"),      Keyword::NIL},
        {("not"),      Keyword::NOT},
        {("or"),       Keyword::OR},
        {("repeat"),   Keyword::REPEAT},
        {("return"),   Keyword::RETURN},
        {("then"),     Keyword::THEN},
        {("true"),     Keyword::TRUE},
        {("until"),    Keyword::UNTIL},
        {("while"),    Keyword::WHILE}
};

const std::unordered_map<std::string, Operator> operators {
        {("+"), Operator::PLUS},
        {("-"), Operator::MINUS},
        {("*"), Operator::ASTERISK},
        {("/"), Operator::SLASH},
        {("%"), Operator::PERCENT},
        {("^"), Operator::CARET},
        {("#"), Operator::SHARP},
        {("&"), Operator::AMPERSAND},
        {("~"), Operator::TILDA},
        {("|"), Operator::VERTICAL_BAR},
        {("<<"), Operator::LT_LT},
        {(">>"), Operator::GT_GT},
        {("//"), Operator::SLASH_SLASH},
        {("=="), Operator::EQUAL_EQUAL},
        {("~="), Operator::TILDA_EQUAL},
        {("<="), Operator::LT_EQUAL},
        {(">="), Operator::GT_EQUAL},
        {("<"), Operator::LESS_THAN},
        {(">"), Operator::GREATER_THAN},
        {("="), Operator::EQUAL},
        {("("), Operator::LEFT_PARENTHESIS},
        {(")"), Operator::RIGHT_PARENTHESIS},
        {("["), Operator::LEFT_BRACKET},
        {("]"), Operator::RIGHT_BRACKET},
        {("{"), Operator::LEFT_CURLY_BRACE},
        {("}"), Operator::RIGHT_CURLY_BRACE},
        {("::"), Operator::COLON_COLON},
        {(";"), Operator::SEMI_COLON},
        {(":"), Operator::COLON},
        {(","), Operator::COMMA},
        {("."), Operator::DOT},
        {(".."), Operator::DOT_DOT},
        {("..."), Operator::DOT_DOT_DOT}
};

bool aux::ir::tokens::isKeyword(const std::string &str){
    return keywords.contains(str);
}

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

std::string TokenIdentifier::getRawValue() const {
    return getValue();
}

TokenKeyword::TokenKeyword(const std::string &value, const Span &span) : Token(span), _keyword(keywords.at(value)) {}

TokenType TokenKeyword::getType() const {
    return TokenType::KEYWORD;
}

const Keyword &TokenKeyword::getKeyword() {
    return this->_keyword;
}

std::string TokenKeyword::getRawValue() const {
    return *this->_keyword;
}

TokenStringLiteral::TokenStringLiteral(std::string value, const Span &span) : Token(span), _value(std::move(value)) {}

TokenType TokenStringLiteral::getType() const {
    return TokenType::STRING_LITERAL;
}

const std::string &TokenStringLiteral::getValue() const {
    return _value;
}

std::string TokenStringLiteral::getRawValue() const {
    return getValue();
}

TokenType TokenOperator::getType() const {
    return TokenType::OPERATOR;
}

TokenOperator::TokenOperator(const std::string &value, const Span &span)
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


TokenComment::TokenComment(std::string value, const Span &span)
        : Token(span), _value(std::move(value)) {}

TokenType TokenComment::getType() const {
    return TokenType::COMMENT;
}

const std::string &TokenComment::getValue() const {
    return _value;
}

std::string TokenComment::getRawValue() const {
    return getValue();
}

