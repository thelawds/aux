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

    inline std::string &operator*(const TokenType &type) {
        static std::unordered_map<TokenType, std::string> keywords {
                {TokenType::IDENTIFIER, "Identifier"},
                {TokenType::KEYWORD, "Keyword"},
                {TokenType::NUMERIC_DECIMAL, "Numerical Decimal"},
                {TokenType::NUMERIC_HEX, "Numerical Hex"},
                {TokenType::NUMERIC_DOUBLE, "Numerical Double"},
                {TokenType::STRING_LITERAL, "String Literal"},
                {TokenType::OPERATOR, "Operator"},
                {TokenType::COMMENT, "Comment"},
                {TokenType::UNDEFINED, "Undefined"}
        };

        return keywords.at(type);
    }

    enum class Keyword {
        AND, BREAK, DO, ELSE, ELSEIF, END, FALSE, FOR, FUNCTION, GOTO, IF,
        IN, LOCAL, NIL, NOT, OR, REPEAT, RETURN, THEN, TRUE, UNTIL, WHILE
    };

    inline std::string &operator*(const Keyword &keyword) {
        static std::unordered_map<Keyword, std::string> keywords {
                {Keyword::AND, ("and")},
                {Keyword::BREAK, ("break")},
                {Keyword::DO, ("do")},
                {Keyword::ELSE, ("else")},
                {Keyword::ELSEIF, ("elseif")},
                {Keyword::END, ("end")},
                {Keyword::FALSE, ("false")},
                {Keyword::FOR, ("for")},
                {Keyword::FUNCTION, ("function")},
                {Keyword::GOTO, ("goto")},
                {Keyword::IF, ("if")},
                {Keyword::IN, ("in")},
                {Keyword::LOCAL, ("local")},
                {Keyword::NIL, ("nil")},
                {Keyword::NOT, ("not")},
                {Keyword::OR, ("or")},
                {Keyword::REPEAT, ("repeat")},
                {Keyword::RETURN, ("return")},
                {Keyword::THEN, ("then")},
                {Keyword::TRUE, ("true")},
                {Keyword::UNTIL, ("until")},
                {Keyword::WHILE, ("while")}
        };

        return keywords.at(keyword);
    }

    enum class Operator {
        PLUS, MINUS, ASTERISK, SLASH, PERCENT, CARET, SHARP, AMPERSAND, TILDA, VERTICAL_BAR,
        LT_LT, GT_GT, SLASH_SLASH, EQUAL_EQUAL, TILDA_EQUAL, LT_EQUAL, GT_EQUAL, LESS_THAN, GREATER_THAN,
        EQUAL, LEFT_PARENTHESIS, RIGHT_PARENTHESIS, LEFT_BRACKET, RIGHT_BRACKET, LEFT_CURLY_BRACE, RIGHT_CURLY_BRACE,
        COLON_COLON, SEMI_COLON, COLON, COMMA, DOT, DOT_DOT, DOT_DOT_DOT
    };

    inline std::string &operator*(const Operator &op) {
        static std::unordered_map<Operator, std::string> operators {
                {Operator::PLUS, ("+")},
                {Operator::MINUS, ("-")},
                {Operator::ASTERISK, ("*")},
                {Operator::SLASH, ("/")},
                {Operator::PERCENT, ("%")},
                {Operator::CARET, ("^")},
                {Operator::SHARP, ("#")},
                {Operator::AMPERSAND, ("&")},
                {Operator::TILDA, ("~")},
                {Operator::VERTICAL_BAR, ("|")},
                {Operator::LT_LT, ("<<")},
                {Operator::GT_GT, (">>")},
                {Operator::SLASH_SLASH, ("//")},
                {Operator::EQUAL_EQUAL, ("==")},
                {Operator::TILDA_EQUAL, ("~=")},
                {Operator::LT_EQUAL, ("<=")},
                {Operator::GT_EQUAL, (">=")},
                {Operator::LESS_THAN, ("<")},
                {Operator::GREATER_THAN, (">")},
                {Operator::EQUAL, ("=")},
                {Operator::LEFT_PARENTHESIS, ("(")},
                {Operator::RIGHT_PARENTHESIS, (")")},
                {Operator::LEFT_BRACKET, ("[")},
                {Operator::RIGHT_BRACKET, ("]")},
                {Operator::LEFT_CURLY_BRACE, ("{")},
                {Operator::RIGHT_CURLY_BRACE, ("}")},
                {Operator::COLON_COLON, ("::")},
                {Operator::SEMI_COLON, (";")},
                {Operator::COLON, (":")},
                {Operator::COMMA, (",")},
                {Operator::DOT, (".")},
                {Operator::DOT_DOT, ("..")},
                {Operator::DOT_DOT_DOT, ("...")}
        };

        return operators.at(op);
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

    template<typename T, Function<const CommonStringType &, T> converter, TokenType TokType>
    struct TokenNumeric : Token {
        TokenNumeric(CommonStringType value, const Span &span)
                : Token(span), _string_value(value), _value(converter(value)) {}

        [[nodiscard]]
        inline TokenType getType() const override {
            return TokType;
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