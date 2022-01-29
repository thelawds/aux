//
// Created by miserable on 29.01.2022.
//

#ifndef AUX_PATTERNMATCHINGEXCEPTION_H
#define AUX_PATTERNMATCHINGEXCEPTION_H

#include <stdexcept>

namespace aux::fsa {

    struct PatternMatchingException : std::runtime_error {
        explicit PatternMatchingException(const std::string &arg) : runtime_error(arg) {}
        explicit PatternMatchingException(const char *string) : runtime_error(string) {}
    };

}



#endif //AUX_PATTERNMATCHINGEXCEPTION_H
