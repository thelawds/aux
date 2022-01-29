//
// Created by miserable on 24.01.2022.
//

#ifndef AUX_ISCANNER_H
#define AUX_ISCANNER_H

#include <vector>
#include <fstream>
#include "../intermediate_representation/tokens/Token.h"

namespace aux::scanner {

    struct IScanner {

        [[nodiscard]] virtual std::vector<aux::ir::tokens::Token *> scan() const = 0;

    };

}


#endif //AUX_ISCANNER_H
