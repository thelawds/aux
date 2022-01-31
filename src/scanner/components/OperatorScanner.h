//
// Created by miserable on 30.01.2022.
//

#ifndef AUX_OPERATORSCANNER_H
#define AUX_OPERATORSCANNER_H

#include "IScannerComponent.h"
#include "../input_stream/IIndexedStream.h"
#include "../../util/Defines.h"
#include <istream>

namespace aux::scanner::components {

    struct OperatorScanner : IScannerComponent {

        explicit OperatorScanner(input_stream::IIndexedStream<CommonCharType> &stream) : _stream(stream) {}

        [[nodiscard]]
        ScanTokenResult next() const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        input_stream::IIndexedStream<CommonCharType> &_stream;

        [[nodiscard]]
        CommonStringType tryScanOperatorOrDelimiter() const;
    };

}

#endif //AUX_OPERATORSCANNER_H
