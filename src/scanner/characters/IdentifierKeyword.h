//
// Created by miserable on 31.01.2022.
//

#ifndef AUX_IDENTIFIERKEYWORD_H
#define AUX_IDENTIFIERKEYWORD_H

#include <string>
#include <unordered_set>

namespace aux::scanner::characters {

    enum class IdentifierKeywordCharType {
        ALPHABETIC,
        NUMERIC,
        UNDERSCORE,
        ALPHABETIC_UNDERSCORE,
        ANY
    };

    inline bool operator>>=(const CommonCharType &c, IdentifierKeywordCharType type){
        switch (type) {
            case IdentifierKeywordCharType::ALPHABETIC:
                return std::isalpha(c);
            case IdentifierKeywordCharType::NUMERIC:
                return std::isdigit(c);
            case IdentifierKeywordCharType::UNDERSCORE:
                return c == '_';
            case IdentifierKeywordCharType::ALPHABETIC_UNDERSCORE:
                return c == '_' || std::isalpha(c);
            case IdentifierKeywordCharType::ANY:
                return c == '_' || std::isalnum(c);
            default:
                return false;
        }
    }

    template<IdentifierKeywordCharType type>
    bool satisfies(CommonCharType c){
        return c >>= type;
    }

}

#define identifier(_Name) aux::scanner::characters::satisfies<aux::scanner::characters::IdentifierKeywordCharType::_Name>

#endif //AUX_IDENTIFIERKEYWORD_H
