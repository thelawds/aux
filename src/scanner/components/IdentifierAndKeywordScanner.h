//
// Created by miserable on 31.01.2022.
//

#ifndef AUX_IDENTIFIERANDKEYWORDSCANNER_H
#define AUX_IDENTIFIERANDKEYWORDSCANNER_H

#include "IScannerComponent.h"
#include <istream>
#include "../fsa/State.h"
#include "../input_stream/IIndexedStream.h"
#include <map>
#include <string>

namespace aux::scanner::components {

    struct IdentifierAndKeywordScanner : IScannerComponent{

        explicit IdentifierAndKeywordScanner(input_stream::IIndexedStream<char> &stream);

        [[nodiscard]]
        ScanTokenResult next() const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        input_stream::IIndexedStream<char> &_stream;
        std::shared_ptr<fsa::State<std::string>> _startingState;

    };
}

#endif //AUX_IDENTIFIERANDKEYWORDSCANNER_H
