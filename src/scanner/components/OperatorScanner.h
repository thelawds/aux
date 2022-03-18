//
// Created by miserable on 30.01.2022.
//

#ifndef AUX_OPERATORSCANNER_H
#define AUX_OPERATORSCANNER_H

#include "IScannerComponent.h"
#include "scanner/input_stream/IIndexedStream.h"
#include <istream>

namespace aux::scanner::components {

    struct OperatorScanner : IScannerComponent {

        explicit OperatorScanner(input_stream::IIndexedStream<char> &stream) : _stream(stream) {}

        [[nodiscard]]
        ScanTokenResult next() const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        input_stream::IIndexedStream<char> &_stream;

        [[nodiscard]]
        std::string tryScanOperatorOrDelimiter() const;
    };

}

#endif //AUX_OPERATORSCANNER_H
