//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_NUMERICCONSTANTSDFSASCANNER_H
#define AUX_NUMERICCONSTANTSDFSASCANNER_H

#include "IScannerComponent.h"
#include <istream>
#include "scanner/fsa/State.h"
#include "scanner/input_stream/IIndexedStream.h"
#include <map>
#include <string>

namespace aux::scanner::components{

    struct NumericConstantsDFSAScanner : IScannerComponent {
        explicit NumericConstantsDFSAScanner(input_stream::IIndexedStream<char> &stream);

        [[nodiscard]]
        ScanTokenResult next() const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        input_stream::IIndexedStream<char> &_stream;
        std::shared_ptr<fsa::State<std::string>> _startingState;

    };

}


#endif //AUX_NUMERICCONSTANTSDFSASCANNER_H
