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
        using ConstructingFunction = BiFunction<const CommonStringType&, const ir::tokens::Span&, std::shared_ptr<ir::tokens::Token>>;

        ScanTokenResult(CommonStringType token, ConstructingFunction constructingFunction);

        IMPLICIT ScanTokenResult(std::runtime_error &runtimeError); // NOLINT(google-explicit-constructor)

        IMPLICIT operator bool() const; // NOLINT(google-explicit-constructor)

        [[nodiscard]]
        std::shared_ptr<std::runtime_error> getScannerError() const;

        [[nodiscard]]
        CommonStringType getToken() const;

        [[nodiscard]]
        std::shared_ptr<ir::tokens::Token> construct(const aux::ir::tokens::Span &span) const;

    private:
        const bool _result;
        const CommonStringType _token;
        const std::shared_ptr<std::runtime_error> _scannerError;
        const ConstructingFunction _constructionFunction;
    };


    template<typename ResultType>
    std::shared_ptr<aux::ir::tokens::Token> makeTokenSharedPtr(const CommonStringType &s, const aux::ir::tokens::Span &span){
        return make_shared<ResultType>(s, span);
    }

}

#endif //AUX_SCANTOKENRESULT_H
