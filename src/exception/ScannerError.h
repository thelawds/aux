//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_SCANNERERROR_H
#define AUX_SCANNERERROR_H


#include <ostream>
#include "../intermediate_representation/tokens/Token.h"

namespace aux::exception {

    struct ScannerError {
        friend std::ostream &operator<<(std::ostream &os, const ScannerError &error) {
            return os;
        }

        explicit ScannerError(const ir::tokens::Span &span) : _span(span) {}

    private:
        const ir::tokens::Span _span;
    };

}

#endif //AUX_SCANNERERROR_H
