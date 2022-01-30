//
// Created by miserable on 24.01.2022.
//

#ifndef AUX_TOKEN_H
#define AUX_TOKEN_H

#include <string>
#include <utility>
#include <unordered_map>
#include "../../util/Defines.h"

namespace aux::ir::tokens {

    enum class TokenType {
        IDENTIFIER,
        KEYWORD,
        NUMERIC,
        STRING_LITERAL,
        OPERATOR,
        DELIMITER,
        COMMENT
    };

    enum class Keyword {
        AND,
        BREAK,
        DO,
        ELSE,
        ELSEIF,
        END,
        FALSE,
        FOR,
        FUNCTION,
        GOTO,
        IF,
        IN,
        LOCAL,
        NIL,
        NOT,
        OR,
        REPEAT,
        RETURN,
        THEN,
        TRUE,
        UNTIL,
        WHILE
    };

    inline const std::unordered_map<std::string, Keyword> keywords{
            {"and",      Keyword::AND},
            {"break",    Keyword::BREAK},
            {"do",       Keyword::DO},
            {"else",     Keyword::ELSE},
            {"elseif",   Keyword::ELSEIF},
            {"end",      Keyword::END},
            {"false",    Keyword::FALSE},
            {"for",      Keyword::FOR},
            {"function", Keyword::FUNCTION},
            {"goto",     Keyword::GOTO},
            {"if",       Keyword::IF},
            {"in",       Keyword::IN},
            {"local",    Keyword::LOCAL},
            {"nil",      Keyword::NIL},
            {"not",      Keyword::NOT},
            {"or",       Keyword::OR},
            {"repeat",   Keyword::REPEAT},
            {"return",   Keyword::RETURN},
            {"then",     Keyword::THEN},
            {"true",     Keyword::TRUE},
            {"until",    Keyword::UNTIL},
            {"while",    Keyword::WHILE}
    };

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

        virtual ~Token();

    private:
        const Span _span;

    };

    struct TokenIdentifier : Token {
        TokenIdentifier(std::string value, const Span &span);

        [[nodiscard]] TokenType getType() const override;

        [[nodiscard]] const std::string &getValue() const;

        ~TokenIdentifier() override;

    private:
        const std::string _value;
    };

    struct TokenKeyword : Token {
        TokenKeyword(const std::string &value, const Span &span);

        [[nodiscard]] TokenType getType() const override;

        const Keyword &getKeyword();

        ~TokenKeyword() override;

    private:
        const Keyword _keyword;
    };

    template<typename T, Function<const std::string&, T> converter>
    struct TokenNumeric : Token {
        TokenNumeric(std::string value, const Span &span)
                : Token(span), _string_value(value), _value(converter(value)) {}

        [[nodiscard]]
        inline TokenType getType() const override {
            return TokenType::NUMERIC;
        };

        inline T getValue() const {
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

    template<uint16_t base>
    inline uint64_t toInteger(const std::string& string){
        return std::stoll(string, nullptr, base);
    }

    inline long double toLongDouble(const std::string &string){
        return std::stold(string);
    }

    using TokenDecimal = TokenNumeric<uint64_t, toInteger<10>>;
    using TokenHex = TokenNumeric<uint64_t, toInteger<16>>;
    using TokenDouble = TokenNumeric<long double, toLongDouble>;

    // todo: finish all after that line:
    // -----------------------------

    struct TokenStringLiteral : Token {
        TokenStringLiteral(std::string value, const Span & span);

        [[nodiscard]]
        TokenType getType() const override;

        [[nodiscard]]
        std::string getValue() const;
    private:
        std::string _value;
    };

    struct TokenOperator : Token {
        explicit TokenOperator(const Span &span);

        [[nodiscard]] TokenType getType() const override;
    };

    struct TokenDelimiter : Token {
        explicit TokenDelimiter(const Span &span);

        [[nodiscard]] TokenType getType() const override;
    };

}

#endif //AUX_TOKEN_H
