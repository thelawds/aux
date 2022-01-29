//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_NUMERIC_H
#define AUX_NUMERIC_H

#include <string>
#include "Delimiters.h"

namespace aux::scanner::components::characters {

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

}

inline bool operator>>=(const char &c, aux::scanner::components::characters::NumericCharType type) {
    switch (type) {
        case aux::scanner::components::characters::NumericCharType::ZERO:
            return c == '0';
        case aux::scanner::components::characters::NumericCharType::ALL_DECIMAL:
            return std::isdigit(c);
        case aux::scanner::components::characters::NumericCharType::POSITIVE_DECIMAL:
            return c != '0' && std::isdigit(c);
        case aux::scanner::components::characters::NumericCharType::ALL_HEX:
            return std::isdigit(c) || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
        case aux::scanner::components::characters::NumericCharType::POSITIVE_HEX:
            return (c >>= aux::scanner::components::characters::NumericCharType::ALL_HEX) && c != '0';
        case aux::scanner::components::characters::NumericCharType::HEX_DELIM:
            return c == 'x' || c == 'X';
        case aux::scanner::components::characters::NumericCharType::DECIMAL_EXP:
            return c == 'e' || c == 'E';
        case aux::scanner::components::characters::NumericCharType::HEX_EXP:
            return c == 'p' || c == 'P';
        case aux::scanner::components::characters::NumericCharType::PLUS_MINUS:
            return c == '+' || c == '-';
        case aux::scanner::components::characters::NumericCharType::HEX_DECIMAL_FLOATING_POINT:
            return c == '.';
        case aux::scanner::components::characters::NumericCharType::ANY:
            return (c >>= aux::scanner::components::characters::NumericCharType::ALL_HEX)
                   || (c == 'x' || c == 'X') || (c == 'e' || c == 'E') || (c == 'p' || c == 'P')
                   || (c == '.') || (c == '+' || c == '-');
        default:
            return false;
    }
}

template<aux::scanner::components::characters::NumericCharType type>
bool satisfies(char c) {
    return c >>= type;
}

#endif //AUX_NUMERIC_H