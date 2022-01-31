//
// Created by miserable on 24.01.2022.
//

#ifndef AUX_TOKEN_H
#define AUX_TOKEN_H

#include <string>
#include <utility>
#include <unordered_map>
#include <memory>
#include "../../util/Defines.h"

namespace aux::ir::tokens {

    enum class TokenType {
        IDENTIFIER,
        KEYWORD,
        NUMERIC,
        STRING_LITERAL,
        OPERATOR,
        COMMENT,
        UNDEFINED
    };

    enum class Keyword {
        AND, BREAK, DO, ELSE, ELSEIF, END, FALSE, FOR, FUNCTION, GOTO, IF,
        IN, LOCAL, NIL, NOT, OR, REPEAT, RETURN, THEN, TRUE, UNTIL, WHILE
    };

    enum class Operator {
        PLUS, MINUS, ASTERISK, SLASH, PERCENT, CARET, SHARP, AMPERSAND, TILDA, VERTICAL_BAR,
        LT_LT, GT_GT, SLASH_SLASH, EQUAL_EQUAL, TILDA_EQUAL, LT_EQUAL, GT_EQUAL, LESS_THAN, GREATER_THAN,
        EQUAL, LEFT_PARENTHESIS, RIGHT_PARENTHESIS, LEFT_BRACKET, RIGHT_BRACKET, LEFT_CURLY_BRACE, RIGHT_CURLY_BRACE,
        COLON_COLON, SEMI_COLON, COLON, COMMA, DOT, DOT_DOT, DOT_DOT_DOT
    };

    bool isKeyword(const std::string &str);

    struct Span {
        const uint32_t column;
        const uint32_t row;

        Span(const uint32_t column, const uint32_t row) : row(row), column(column) {}
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

        TokenComment(std::string value, const Span& span);

        [[nodiscard]]
        TokenType getType() const override;

        [[nodiscard]]
        const std::string &getValue() const;

    private:
        std::string _value;
    };

    struct TokenIdentifier : Token {
        TokenIdentifier(std::string value, const Span &span);

        [[nodiscard]]
        TokenType getType() const override;

        [[nodiscard]]
        const std::string &getValue() const;

    private:
        const std::string _value;
    };

    struct TokenKeyword : Token {
        TokenKeyword(const std::string &value, const Span &span);

        [[nodiscard]]
        TokenType getType() const override;

        const Keyword &getKeyword();

    private:
        const Keyword _keyword;
    };

    template<typename T, Function<const std::string &, T> converter>
    struct TokenNumeric : Token {
        TokenNumeric(std::string value, const Span &span)
                : Token(span), _string_value(value), _value(converter(value)) {}

        [[nodiscard]]
        inline TokenType getType() const override {
            return TokenType::NUMERIC;
        };

        inline const T &getValue() const {
            return _value;
        };

        [[nodiscard]]
        inline std::string getRawValue() const {
            return _string_value;
        }

    private:
        T _value;
        std::string _string_value;
    };


    struct TokenStringLiteral : Token {
        TokenStringLiteral(std::string value, const Span &span);

        [[nodiscard]]
        TokenType getType() const override;

        [[nodiscard]]
        const std::string &getValue() const;

    private:
        std::string _value;
    };

    struct TokenOperator : Token {
        explicit TokenOperator(const std::string &value, const Span &span);

        [[nodiscard]]
        TokenType getType() const override;

        [[nodiscard]]
        const Operator &getValue() const;

    private:
        Operator _value;
    };

    template<uint16_t base>
    inline uint64_t toInteger(const std::string &string) {
        return std::stoll(string, nullptr, base);
    }

    inline long double toLongDouble(const std::string &string) {
        return std::stold(string);
    }

    using TokenDecimal = TokenNumeric<uint64_t, toInteger<10>>;
    using TokenHex = TokenNumeric<uint64_t, toInteger<16>>;
    using TokenDouble = TokenNumeric<long double, toLongDouble>;
}

#endif //AUX_TOKEN_H
