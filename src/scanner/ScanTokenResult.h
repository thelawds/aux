//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_SCANTOKENRESULT_H
#define AUX_SCANTOKENRESULT_H

#include <string>
#include "../exception/ScannerError.h"
#include "../intermediate_representation/tokens/Token.h"
#include "../util/Defines.h"

namespace aux::scanner{

    struct ScanTokenResult {
        ScanTokenResult(
                bool result,
                exception::ScannerError *scannerError,
                ir::tokens::Token *token
        );

        IMPLICIT ScanTokenResult(exception::ScannerError *scannerError);

        IMPLICIT ScanTokenResult(ir::tokens::Token *token);

        IMPLICIT operator bool() const; // NOLINT(google-explicit-constructor)

        [[nodiscard]]
        aux::exception::ScannerError *getScannerError() const;

        [[nodiscard]]
        aux::ir::tokens::Token *getToken() const;

    private:
        const bool _result;
        aux::exception::ScannerError *const _scannerError;
        aux::ir::tokens::Token *const _token;
    };

}


#endif //AUX_SCANTOKENRESULT_H
