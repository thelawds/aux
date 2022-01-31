//
// Created by miserable on 31.01.2022.
//

#ifndef AUX_COMMENTSSCANNER_H
#define AUX_COMMENTSSCANNER_H

#include "IScannerComponent.h"
#include <istream>
#include "../../fsa/State.h"
#include "../../exception/PatternMatchingException.h"
#include <map>
#include <string>

namespace aux::scanner::components {

    struct CommentsScanner :IScannerComponent {

        explicit CommentsScanner(std::istream &stream);

        [[nodiscard]]
        ScanTokenResult next(ir::tokens::Span span) const override;

        [[nodiscard]]
        bool canProcessNextToken() const override;

    private:
        std::istream &_stream;
    };

}



#endif //AUX_COMMENTSSCANNER_H
