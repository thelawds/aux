//
// Created by miserable on 31.01.2022.
//

#ifndef AUX_IDENTIFIERANDKEYWORDSCANNER_H
#define AUX_IDENTIFIERANDKEYWORDSCANNER_H

#include "IScannerComponent.h"
#include <istream>
#include "../../fsa/State.h"
#include "../../exception/PatternMatchingException.h"
#include <map>
#include <string>

namespace aux::scanner::components {

    struct IdentifierAndKeywordScanner : IScannerComponent{

        explicit IdentifierAndKeywordScanner(std::istream &stream);

        [[nodiscard]]
        ScanTokenResult next(ir::tokens::Span span) const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        std::istream &_stream;
        std::shared_ptr<fsa::State<std::string>> _startingState;

    };
}

#endif //AUX_IDENTIFIERANDKEYWORDSCANNER_H
