//
// Created by miserable on 29.01.2022.
//

#ifndef AUX_DELIMITERS_H
#define AUX_DELIMITERS_H

#include <string>
#include <unordered_set>

namespace aux::scanner::characters {

    enum class DelimiterCharType {
        OPERATOR,
        DELIMITER,
        SPACE,
        END_OF_FILE,
        ANY
    };


    inline bool operator>>=(const char &c, DelimiterCharType type) {
        static std::unordered_set<char> operators{'+', '-', '*', '/', '%', '^', '#', '&', '~', '|', '<', '>', '='};
        static std::unordered_set<char> delimiters{'(', ')', '{', '}', '[', ']', ':', ';', ',', '.'};

        switch (type) {
            case DelimiterCharType::OPERATOR:
                return operators.contains(c);
            case DelimiterCharType::DELIMITER:
                return delimiters.contains(c);
            case DelimiterCharType::SPACE:
                return std::isspace(c);
            case DelimiterCharType::END_OF_FILE:
                return c == std::char_traits<char>::eof();
            case DelimiterCharType::ANY:
                return (c >>= DelimiterCharType::OPERATOR)
                       || (c >>= DelimiterCharType::DELIMITER)
                       || (c >>= DelimiterCharType::SPACE)
                       || (c >>= DelimiterCharType::END_OF_FILE);
            default:
                return false;
        }
    }

    template<DelimiterCharType type>
    bool satisfies(char c) {
        return c >>= type;
    }

}

#define delimiter(_Name) satisfies<DelimiterCharType::_Name>

#endif //AUX_DELIMITERS_H
