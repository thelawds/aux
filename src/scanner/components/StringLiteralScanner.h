//
// Created by miserable on 30.01.2022.
//

#ifndef AUX_STRINGLITERALSCANNER_H
#define AUX_STRINGLITERALSCANNER_H

#include "IScannerComponent.h"
#include <istream>
#include "../../fsa/State.h"
#include "../../fsa/PatternMatchingException.h"

namespace aux::scanner::components {

    struct StringLiteralScanner : IScannerComponent {

        explicit StringLiteralScanner(std::istream &stream);

        [[nodiscard]]
        ScanTokenResult next(ir::tokens::Span span) const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        std::istream &_stream;
        std::shared_ptr<fsa::State<std::string>> _startingState;

        [[nodiscard]]
        ScanTokenResult readWithLongBracket(ir::tokens::Span span) const;

    };
}



#endif //AUX_STRINGLITERALSCANNER_H
