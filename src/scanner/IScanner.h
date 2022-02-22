//
// Created by miserable on 24.01.2022.
//

#ifndef AUX_ISCANNER_H
#define AUX_ISCANNER_H

#include <vector>
#include <fstream>
#include <memory>
#include "../intermediate_representation/Token.h"

namespace aux::scanner {

    struct IScanner {

        /**
         * @pure
         * If there is no tokens left, should return object of @class ir::tokens::UndefinedToken
         */
        [[nodiscard]]
        virtual std::shared_ptr<ir::tokens::Token> next() const = 0;

        /**
         * @pure
         * If there is no tokens left, should return object of @class ir::tokens::UndefinedToken
         */
        [[nodiscard]]
        virtual std::shared_ptr<ir::tokens::Token> peek() const = 0;

    };

}


#endif //AUX_ISCANNER_H
