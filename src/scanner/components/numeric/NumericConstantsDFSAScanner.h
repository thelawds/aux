//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_NUMERICCONSTANTSDFSASCANNER_H
#define AUX_NUMERICCONSTANTSDFSASCANNER_H

#include "../../ScanTokenResult.h"
#include "../../characters/Numeric.h"
#include "../../../finite_state_automata/State.h"
#include "../../../finite_state_automata/PatternMatchingException.h"
#include <map>
#include <string>

namespace aux::scanner::components::numeric {

    struct NumericConstantsDFSAScanner {
        explicit NumericConstantsDFSAScanner(std::istream &stream);

        [[nodiscard]] ScanTokenResult next() const;

    private:
        std::istream &_stream;
        std::shared_ptr<fsa::State<std::string>> _startingState;

    };

}


#endif //AUX_NUMERICCONSTANTSDFSASCANNER_H
