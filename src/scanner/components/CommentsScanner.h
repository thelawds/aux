//
// Created by miserable on 31.01.2022.
//

#ifndef AUX_COMMENTSSCANNER_H
#define AUX_COMMENTSSCANNER_H

#include "IScannerComponent.h"
#include <istream>
#include "../fsa/State.h"
#include "../input_stream/IIndexedStream.h"
#include <map>
#include <string>

namespace aux::scanner::components {

    struct CommentsScanner :IScannerComponent {

        explicit CommentsScanner(input_stream::IIndexedStream<char> &stream);

        [[nodiscard]]
        ScanTokenResult next() const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        input_stream::IIndexedStream<char> &_stream;
    };

}



#endif //AUX_COMMENTSSCANNER_H
