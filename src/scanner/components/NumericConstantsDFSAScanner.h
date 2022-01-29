//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_NUMERICCONSTANTSDFSASCANNER_H
#define AUX_NUMERICCONSTANTSDFSASCANNER_H

#include "IScannerComponent.h"
#include "../../finite_state_automata/State.h"
#include "../../finite_state_automata/PatternMatchingException.h"
#include <map>
#include <string>

namespace aux::scanner::components{

    struct NumericConstantsDFSAScanner : IScannerComponent{
        explicit NumericConstantsDFSAScanner(std::istream &stream);

        [[nodiscard]]
        ScanTokenResult next(ir::tokens::Span span) const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        std::istream &_stream;
        std::shared_ptr<fsa::State<std::string>> _startingState;

    };

}


#endif //AUX_NUMERICCONSTANTSDFSASCANNER_H
