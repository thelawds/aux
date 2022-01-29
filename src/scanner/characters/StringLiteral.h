//
// Created by miserable on 30.01.2022.
//

#ifndef AUX_STRINGLITERAL_H
#define AUX_STRINGLITERAL_H

#include <string>

namespace aux::scanner::characters {

    enum class StringLiteralCharType {
        SINGLE_QUOTE,
        DOUBLE_QUOTE,
        BACKSLASH,
        NON_EOF,
        ANY
    };

    inline bool operator>>=(const CommonCharType &c, StringLiteralCharType type){
        switch (type) {
            case StringLiteralCharType::SINGLE_QUOTE:
                return c == '\'';
            case StringLiteralCharType::DOUBLE_QUOTE:
                return c == '\"';
            case StringLiteralCharType::BACKSLASH:
                return c == '\\';
            case StringLiteralCharType::NON_EOF:
                return c != std::char_traits<CommonCharType>::eof();
            case StringLiteralCharType::ANY:
                return (c >>= StringLiteralCharType::SINGLE_QUOTE)
                       || (c >>= StringLiteralCharType::DOUBLE_QUOTE)
                       || (c >>= StringLiteralCharType::BACKSLASH)
                       || (c >>= StringLiteralCharType::NON_EOF);
            default:
                return false;
        }
    }


    template<StringLiteralCharType type>
    bool satisfies(CommonCharType c) {
        return c >>= type;
    }

}

#define stringLiteral(_Name) aux::scanner::characters::satisfies<aux::scanner::characters::StringLiteralCharType::_Name>

#endif //AUX_STRINGLITERAL_H
