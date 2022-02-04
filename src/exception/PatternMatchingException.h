//
// Created by miserable on 29.01.2022.
//

#ifndef AUX_PATTERNMATCHINGEXCEPTION_H
#define AUX_PATTERNMATCHINGEXCEPTION_H

#include <stdexcept>

namespace aux::fsa {

    struct PatternMatchingException : std::runtime_error {
        PatternMatchingException(const std::string &exception, char errorAt)
                : std::runtime_error(exception + " at " + std::string(1, errorAt)) {}

        PatternMatchingException(const char *exception, char errorAt)
                : std::runtime_error(std::string(exception) + " at " + std::string(1, errorAt)) {}

    };

}


#endif //AUX_PATTERNMATCHINGEXCEPTION_H
