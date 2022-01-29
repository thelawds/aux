//
// Created by miserable on 24.01.2022.
//

#ifndef AUX_ISCANNER_H
#define AUX_ISCANNER_H

#include <vector>
#include <fstream>
#include <memory>
#include "../intermediate_representation/tokens/Token.h"

namespace aux::scanner {

    struct IScanner {

        [[nodiscard]]
        virtual std::shared_ptr<ir::tokens::Token> next() const = 0;

    };

}


#endif //AUX_ISCANNER_H
