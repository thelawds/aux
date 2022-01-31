//
// Created by miserable on 24.01.2022.
//

#ifndef AUX_TOKEN_H
#define AUX_TOKEN_H

#include <string>
#include <utility>
#include <unordered_map>
#include <memory>
#include <ostream>
#include "../../util/Defines.h"

namespace aux::ir::tokens {

    enum class TokenType {
        IDENTIFIER,
        KEYWORD,
        NUMERIC_DECIMAL,
        NUMERIC_HEX,
        NUMERIC_DOUBLE,
        STRING_LITERAL,
        OPERATOR,
        COMMENT,
        UNDEFINED
    };

    enum class Keyword {
        AND, BREAK, DO, ELSE, ELSEIF, END, FALSE, FOR, FUNCTION, GOTO, IF,
        IN, LOCAL, NIL, NOT, OR, REPEAT, RETURN, THEN, TRUE, UNTIL, WHILE
    };

    inline std::basic_ostream<CommonCharType> &operator<<(std::basic_ostream<CommonCharType> &os, const Keyword &keyword) {
        static std::unordered_map<Keyword, CommonStringType> keywords {
                {Keyword::AND, toCommonStringType("and")},
                {Keyword::BREAK, toCommonStringType("break")},
                {Keyword::DO, toCommonStringType("do")},
                {Keyword::ELSE, toCommonStringType("else")},
                {Keyword::ELSEIF, toCommonStringType("elseif")},
                {Keyword::END, toCommonStringType("end")},
                {Keyword::FALSE, toCommonStringType("false")},
                {Keyword::FOR, toCommonStringType("for")},
                {Keyword::FUNCTION, toCommonStringType("function")},
                {Keyword::GOTO, toCommonStringType("goto")},
                {Keyword::IF, toCommonStringType("if")},
                {Keyword::IN, toCommonStringType("in")},
                {Keyword::LOCAL, toCommonStringType("local")},
                {Keyword::NIL, toCommonStringType("nil")},
                {Keyword::NOT, toCommonStringType("not")},
                {Keyword::OR, toCommonStringType("or")},
                {Keyword::REPEAT, toCommonStringType("repeat")},
                {Keyword::RETURN, toCommonStringType("return")},
                {Keyword::THEN, toCommonStringType("then")},
                {Keyword::TRUE, toCommonStringType("true")},
                {Keyword::UNTIL, toCommonStringType("until")},
                {Keyword::WHILE, toCommonStringType("while")}
        };

        os << keywords.at(keyword);
        return os;
    }

    enum class Operator {
        PLUS, MINUS, ASTERISK, SLASH, PERCENT, CARET, SHARP, AMPERSAND, TILDA, VERTICAL_BAR,
        LT_LT, GT_GT, SLASH_SLASH, EQUAL_EQUAL, TILDA_EQUAL, LT_EQUAL, GT_EQUAL, LESS_THAN, GREATER_THAN,
        EQUAL, LEFT_PARENTHESIS, RIGHT_PARENTHESIS, LEFT_BRACKET, RIGHT_BRACKET, LEFT_CURLY_BRACE, RIGHT_CURLY_BRACE,
        COLON_COLON, SEMI_COLON, COLON, COMMA, DOT, DOT_DOT, DOT_DOT_DOT
    };

    inline std::basic_ostream<CommonCharType> &operator<<(std::basic_ostream<CommonCharType> &os, const Operator &op) {
        static std::unordered_map<Operator, CommonStringType> operators {
                {Operator::PLUS, toCommonStringType("+")},
                {Operator::MINUS, toCommonStringType("-")},
                {Operator::ASTERISK, toCommonStringType("*")},
                {Operator::SLASH, toCommonStringType("/")},
                {Operator::PERCENT, toCommonStringType("%")},
                {Operator::CARET, toCommonStringType("^")},
                {Operator::SHARP, toCommonStringType("#")},
                {Operator::AMPERSAND, toCommonStringType("&")},
                {Operator::TILDA, toCommonStringType("~")},
                {Operator::VERTICAL_BAR, toCommonStringType("|")},
                {Operator::LT_LT, toCommonStringType("<<")},
                {Operator::GT_GT, toCommonStringType(">>")},
                {Operator::SLASH_SLASH, toCommonStringType("//")},
                {Operator::EQUAL_EQUAL, toCommonStringType("==")},
                {Operator::TILDA_EQUAL, toCommonStringType("~=")},
                {Operator::LT_EQUAL, toCommonStringType("<=")},
                {Operator::GT_EQUAL, toCommonStringType(">=")},
                {Operator::LESS_THAN, toCommonStringType("<")},
                {Operator::GREATER_THAN, toCommonStringType(">")},
                {Operator::EQUAL, toCommonStringType("=")},
                {Operator::LEFT_PARENTHESIS, toCommonStringType("(")},
                {Operator::RIGHT_PARENTHESIS, toCommonStringType(")")},
                {Operator::LEFT_BRACKET, toCommonStringType("[")},
                {Operator::RIGHT_BRACKET, toCommonStringType("]")},
                {Operator::LEFT_CURLY_BRACE, toCommonStringType("{")},
                {Operator::RIGHT_CURLY_BRACE, toCommonStringType("}")},
                {Operator::COLON_COLON, toCommonStringType("::")},
                {Operator::SEMI_COLON, toCommonStringType(";")},
                {Operator::COLON, toCommonStringType(":")},
                {Operator::COMMA, toCommonStringType(",")},
                {Operator::DOT, toCommonStringType(".")},
                {Operator::DOT_DOT, toCommonStringType("..")},
                {Operator::DOT_DOT_DOT, toCommonStringType("...")}
        };

        os << operators.at(op);
        return os;
    }

    bool isKeyword(const CommonStringType &str);

    struct Span {
        const uint16_t row;
        const uint16_t column;

        Span(const uint16_t row, const uint16_t column) : row(row), column(column) {}
    };

    struct Token {
        explicit Token(const Span &span) : _span(span) {}

        [[nodiscard]]
        Span getSpan() const;

        [[nodiscard]]
        virtual TokenType getType() const = 0;

    private:
        const Span _span;

    };

    struct TokenUndefined : Token {
        explicit TokenUndefined(const Span &span);

        [[nodiscard]]
        TokenType getType() const override;
    };

    struct TokenComment : Token {

        TokenComment(CommonStringType value, const Span& span);

        [[nodiscard]]
        TokenType getType() const override;

        [[nodiscard]]
        const CommonStringType &getValue() const;

    private:
        CommonStringType _value;
    };

    struct TokenIdentifier : Token {
        TokenIdentifier(CommonStringType value, const Span &span);

        [[nodiscard]]
        TokenType getType() const override;

        [[nodiscard]]
        const CommonStringType &getValue() const;

    private:
        const CommonStringType _value;
    };

    struct TokenKeyword : Token {
        TokenKeyword(const CommonStringType &value, const Span &span);

        [[nodiscard]]
        TokenType getType() const override;

        const Keyword &getKeyword();

        friend std::ostream &operator<<(std::ostream &os, const TokenKeyword &keyword);

    private:
        const Keyword _keyword;
    };

    template<typename T, Function<const CommonStringType &, T> converter, TokenType _TokType>
    struct TokenNumeric : Token {
        TokenNumeric(CommonStringType value, const Span &span)
                : Token(span), _string_value(value), _value(converter(value)) {}

        [[nodiscard]]
        inline TokenType getType() const override {
            return _TokType;
        };

        inline const T &getValue() const {
            return _value;
        };

        [[nodiscard]]
        inline CommonStringType getRawValue() const {
            return _string_value;
        }

    private:
        T _value;
        CommonStringType _string_value;
    };


    struct TokenStringLiteral : Token {
        TokenStringLiteral(CommonStringType value, const Span &span);

        [[nodiscard]]
        TokenType getType() const override;

        [[nodiscard]]
        const CommonStringType &getValue() const;

    private:
        CommonStringType _value;
    };

    struct TokenOperator : Token {
        explicit TokenOperator(const CommonStringType &value, const Span &span);

        [[nodiscard]]
        TokenType getType() const override;

        [[nodiscard]]
        const Operator &getOperator() const;

    private:
        Operator _value;
    };

    template<uint16_t base>
    inline uint64_t toInteger(const CommonStringType &string) {
        return std::stoll(string, nullptr, base);
    }

    inline long double toLongDouble(const CommonStringType &string) {
        return std::stold(string);
    }

    using TokenDecimal = TokenNumeric<uint64_t, toInteger<10>, TokenType::NUMERIC_DECIMAL>;
    using TokenHex = TokenNumeric<uint64_t, toInteger<16>, TokenType::NUMERIC_HEX>;
    using TokenDouble = TokenNumeric<long double, toLongDouble, TokenType::NUMERIC_DOUBLE>;
}

#endif //AUX_TOKEN_H
