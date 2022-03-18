//
// Created by miserable on 30.01.2022.
//

#ifndef AUX_STRINGLITERALSCANNER_H
#define AUX_STRINGLITERALSCANNER_H

#include "IScannerComponent.h"
#include <istream>
#include "scanner/fsa/State.h"
#include "scanner/input_stream/IIndexedStream.h"

namespace aux::scanner::components {

    struct StringLiteralScanner : IScannerComponent {

        explicit StringLiteralScanner(input_stream::IIndexedStream<char> &stream);

        [[nodiscard]]
        ScanTokenResult next() const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        input_stream::IIndexedStream<char> &_stream;
        std::shared_ptr<fsa::State<std::string>> _startingState;

        [[nodiscard]]
        ScanTokenResult readWithLongBracket() const;

    };
}



#endif //AUX_STRINGLITERALSCANNER_H
