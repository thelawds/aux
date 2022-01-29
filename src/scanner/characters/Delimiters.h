//
// Created by miserable on 29.01.2022.
//

#ifndef AUX_DELIMITERS_H
#define AUX_DELIMITERS_H

#include <string>
#include <unordered_set>

namespace aux::scanner::components::characters {

    enum class DelimiterCharType {
        OPERATOR,
        DELIMITER,
        SPACE,
        END_OF_FILE,
        ANY
    };

}


inline bool operator>>=(const char &c, aux::scanner::components::characters::DelimiterCharType type) {
    static std::unordered_set<char> operators{'+', '-', '*', '/', '%', '^', '#', '&', '~', '|', '<', '>', '='};
    static std::unordered_set<char> delimiters{'(', ')', '{', '}', '[', ']', ':', ';', ',', '.'};

    switch (type) {
        case aux::scanner::components::characters::DelimiterCharType::OPERATOR:
            return operators.contains(c);
        case aux::scanner::components::characters::DelimiterCharType::DELIMITER:
            return delimiters.contains(c);
        case aux::scanner::components::characters::DelimiterCharType::SPACE:
            return std::isspace(c);
        case aux::scanner::components::characters::DelimiterCharType::END_OF_FILE:
            return c == std::char_traits<char>::eof();
        case aux::scanner::components::characters::DelimiterCharType::ANY:
            return (c >>= aux::scanner::components::characters::DelimiterCharType::OPERATOR)
                   || (c >>= aux::scanner::components::characters::DelimiterCharType::DELIMITER)
                   || (c >>= aux::scanner::components::characters::DelimiterCharType::SPACE)
                   || (c >>= aux::scanner::components::characters::DelimiterCharType::END_OF_FILE);
        default:
            return false;
    }
}

template<aux::scanner::components::characters::DelimiterCharType type>
bool satisfies(char c) {
    return c >>= type;
}

#endif //AUX_DELIMITERS_H
