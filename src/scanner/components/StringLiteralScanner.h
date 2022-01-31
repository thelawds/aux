//
// Created by miserable on 30.01.2022.
//

#ifndef AUX_STRINGLITERALSCANNER_H
#define AUX_STRINGLITERALSCANNER_H

#include "IScannerComponent.h"
#include <istream>
#include "../../fsa/State.h"
#include "../../exception/PatternMatchingException.h"
#include "../input_stream/IIndexedStream.h"

namespace aux::scanner::components {

    struct StringLiteralScanner : IScannerComponent {

        explicit StringLiteralScanner(input_stream::IIndexedStream<CommonCharType> &stream);

        [[nodiscard]]
        ScanTokenResult next() const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        input_stream::IIndexedStream<CommonCharType> &_stream;
        std::shared_ptr<fsa::State<CommonStringType>> _startingState;

        [[nodiscard]]
        ScanTokenResult readWithLongBracket() const;

    };
}



#endif //AUX_STRINGLITERALSCANNER_H
