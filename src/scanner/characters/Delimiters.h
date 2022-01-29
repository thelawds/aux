//
// Created by miserable on 29.01.2022.
//

#ifndef AUX_DELIMITERS_H
#define AUX_DELIMITERS_H

#include <string>

namespace aux::scanner::components::characters {

    enum class DelimiterType {
        OPERATOR,
        DELIMITER,
        SPACE
    };

}

#endif //AUX_DELIMITERS_H
