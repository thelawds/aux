//
// Created by miserable on 13.02.2022.
//

#ifndef AUX_EXCEPTION_H
#define AUX_EXCEPTION_H

#include <vector>
#include <stdexcept>
#include "intermediate_representation/Token.h"

namespace aux::exception {

    enum class ParsedConstructionType {
        EXPRESSION,
        STATEMENT
    };

    inline std::string operator*(const ParsedConstructionType &constructionType) {
        return constructionType == ParsedConstructionType::EXPRESSION ? "expression" : "statement";
    }

    struct PatternMatchingException : std::runtime_error { // todo: refactor as it is in Parsing Exception
        PatternMatchingException(const std::string &exception, char errorAt)
                : std::runtime_error(exception + " at " + std::string(1, errorAt)) {}

        PatternMatchingException(const char *exception, char errorAt)
                : std::runtime_error(std::string(exception) + " at " + std::string(1, errorAt)) {}

    };

    class ParsingExceptionBuilder;

    struct ParsingException : std::logic_error {

        inline ParsingException(
                uint32_t row,
                uint32_t column,
                ParsedConstructionType constructionType,
                const std::vector<std::string> &expected,
                const std::string &actual,
                const std::string &currentRow
        ) : logic_error(""),
            row(row),
            column(column),
            constructionType(constructionType),
            expected(expected),
            actual(actual),
            currentRow(currentRow) {

            exceptionMessage += "Syntax error at (" + std::to_string(row) + ":" + std::to_string(column) + ") ";
            exceptionMessage += "when parsing " + *constructionType + ": expected any of [";
            for (int i = 0; i < expected.size(); ++i) {
                exceptionMessage += expected[i];
                if (i == expected.size() - 1) {
                    exceptionMessage += "], ";
                } else {
                    exceptionMessage += ",";
                }
            }
            exceptionMessage += "but got " + actual + "\n";
            exceptionMessage += currentRow + "\n";
            // todo: add arrow pointing to the token beginning
            // todo: move message into ErrorMessages.h
        }

        [[nodiscard]]
        inline const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return exceptionMessage.c_str();
        }

        inline static ParsingExceptionBuilder builder();
        inline static ParsingExceptionBuilder expressionErrorBuilder();
        inline static ParsingExceptionBuilder statementErrorBuilder();

    private:
        uint32_t row, column;
        ParsedConstructionType constructionType;
        std::vector<std::string> expected;
        std::string actual;
        std::string currentRow;

        std::string exceptionMessage;
    };

    class ParsingExceptionBuilder {
    public:

        inline ParsingExceptionBuilder &withSpan(const ir::tokens::Span &span) {
            row = span.row;
            column = span.column;
            return *this;
        }

        inline ParsingExceptionBuilder &withConstructionType(const ParsedConstructionType &type) {
            constructionType = type;
            return *this;
        }

        inline ParsingExceptionBuilder &addExpected(const std::string &expectedOption) {
            expected.push_back(expectedOption);
            return *this;
        }

        inline ParsingExceptionBuilder &withExpected(const std::vector<std::string> &allExpected){
            expected = allExpected;
            return *this;
        }

        inline ParsingExceptionBuilder &withActual(const std::string &got) {
            actual = got;
            return *this;
        }

        inline ParsingExceptionBuilder &withCurrentRow(const std::string &currRow) {
            currentRow = currRow;
            return *this;
        }

        inline ParsingException build() {
            return {row, column, constructionType, expected, actual, currentRow};
        }

    private:
        uint32_t row{0}, column{0};
        ParsedConstructionType constructionType{ParsedConstructionType::EXPRESSION};
        std::vector<std::string> expected;
        std::string actual;
        std::string currentRow;

    };

    ParsingExceptionBuilder ParsingException::builder() {
        return {};
    }

    ParsingExceptionBuilder ParsingException::expressionErrorBuilder() {
        auto result = ParsingExceptionBuilder();
        result.withConstructionType(ParsedConstructionType::EXPRESSION);
        return result;
    }

    ParsingExceptionBuilder ParsingException::statementErrorBuilder() {
        auto result = ParsingExceptionBuilder();
        result.withConstructionType(ParsedConstructionType::STATEMENT);
        return result;
    }

}

#endif //AUX_EXCEPTION_H
