//
// Created by miserable on 30.01.2022.
//

#ifndef AUX_OPERATORSCANNER_H
#define AUX_OPERATORSCANNER_H

#include "IScannerComponent.h"
#include <istream>

namespace aux::scanner::components {

    struct OperatorScanner : IScannerComponent {

        explicit OperatorScanner(std::istream &stream) : _stream(stream) {}

        [[nodiscard]]
        ScanTokenResult next(ir::tokens::Span span) const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        std::istream &_stream;

        [[nodiscard]]
        std::string tryScanOperatorOrDelimiter() const;
    };

}

#endif //AUX_OPERATORSCANNER_H
