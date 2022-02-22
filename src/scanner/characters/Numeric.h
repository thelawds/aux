//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_NUMERIC_H
#define AUX_NUMERIC_H

#include <string>
#include "Delimiters.h"

namespace aux::scanner::characters {

    enum class NumericCharType {
        ZERO,
        ALL_DECIMAL,
        POSITIVE_DECIMAL,
        ALL_HEX,
        POSITIVE_HEX,
        HEX_DELIM,
        DECIMAL_EXP,
        HEX_EXP,
        PLUS_MINUS,
        HEX_DECIMAL_FLOATING_POINT,
        ANY
    };

    inline bool operator>>=(const char &c, NumericCharType type) {
        switch (type) {
            case NumericCharType::ZERO:
                return c == '0';
            case NumericCharType::ALL_DECIMAL:
                return std::isdigit(c);
            case NumericCharType::POSITIVE_DECIMAL:
                return c != '0' && std::isdigit(c);
            case NumericCharType::ALL_HEX:
                return std::isdigit(c) || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
            case NumericCharType::POSITIVE_HEX:
                return (c >>= NumericCharType::ALL_HEX) && c != '0';
            case NumericCharType::HEX_DELIM:
                return c == 'x' || c == 'X';
            case NumericCharType::DECIMAL_EXP:
                return c == 'e' || c == 'E';
            case NumericCharType::HEX_EXP:
                return c == 'p' || c == 'P';
            case NumericCharType::PLUS_MINUS:
                return c == '+' || c == '-';
            case NumericCharType::HEX_DECIMAL_FLOATING_POINT:
                return c == '.';
            case NumericCharType::ANY:
                return (c >>= NumericCharType::ALL_HEX)
                       || (c == 'x' || c == 'X') || (c == 'e' || c == 'E') || (c == 'p' || c == 'P')
                       || (c == '.') || (c == '+' || c == '-');
            default:
                return false;
        }
    }

    template<NumericCharType type>
    bool satisfies(char c) {
        return c >>= type;
    }

}

#define numeric(_Name) aux::scanner::characters::satisfies<aux::scanner::characters::NumericCharType::_Name>

#endif //AUX_NUMERIC_H