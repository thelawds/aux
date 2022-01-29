//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_SCANTOKENRESULT_H
#define AUX_SCANTOKENRESULT_H

#include <string>
#include <memory>
#include "../exception/ScannerError.h"
#include "../intermediate_representation/tokens/Token.h"
#include "../util/Defines.h"

namespace aux::scanner{

    struct ScanTokenResult {
        IMPLICIT ScanTokenResult(std::shared_ptr<exception::ScannerError> scannerError);

        IMPLICIT ScanTokenResult(std::shared_ptr<ir::tokens::Token> token);

        IMPLICIT operator bool() const; // NOLINT(google-explicit-constructor)

        [[nodiscard]]
        std::shared_ptr<aux::exception::ScannerError> getScannerError() const;

        [[nodiscard]]
        std::shared_ptr<aux::ir::tokens::Token> getToken() const;
    private:
        const bool _result;
        std::shared_ptr<aux::exception::ScannerError> _scannerError;
        std::shared_ptr<aux::ir::tokens::Token> _token;
    };

}


#endif //AUX_SCANTOKENRESULT_H
