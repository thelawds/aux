//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_NUMERICCONSTANTSDFSASCANNER_H
#define AUX_NUMERICCONSTANTSDFSASCANNER_H

#include "IScannerComponent.h"
#include <istream>
#include "../../fsa/State.h"
#include "../../exception/PatternMatchingException.h"
#include "../input_stream/IIndexedStream.h"
#include "../../util/Defines.h"
#include <map>
#include <string>

namespace aux::scanner::components{

    struct NumericConstantsDFSAScanner : IScannerComponent {
        explicit NumericConstantsDFSAScanner(input_stream::IIndexedStream<CommonCharType> &stream);

        [[nodiscard]]
        ScanTokenResult next() const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        input_stream::IIndexedStream<CommonCharType> &_stream;
        std::shared_ptr<fsa::State<CommonStringType>> _startingState;

    };

}


#endif //AUX_NUMERICCONSTANTSDFSASCANNER_H
