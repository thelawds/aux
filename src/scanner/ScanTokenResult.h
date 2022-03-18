//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_SCANTOKENRESULT_H
#define AUX_SCANTOKENRESULT_H

#include <string>
#include <memory>
#include "intermediate_representation/Token.h"
#include "util/Defines.h"

namespace aux::scanner {

    template<typename IN1, typename IN2, typename OUT>
    using BiFunction = OUT (*)(IN1, IN2);

    struct ScanTokenResult {
        using ConstructingFunction = BiFunction<
                const std::string &,
                const ir::tokens::Span &,
                std::shared_ptr<ir::tokens::Token>
        >;

        ScanTokenResult(std::string token, ConstructingFunction constructingFunction);

        IMPLICIT ScanTokenResult(std::runtime_error &runtimeError); // NOLINT(google-explicit-constructor)

        IMPLICIT operator bool() const; // NOLINT(google-explicit-constructor)

        [[nodiscard]]
        std::shared_ptr<std::runtime_error> getScannerError() const;

        [[nodiscard]]
        std::string getToken() const;

        [[nodiscard]]
        std::shared_ptr<ir::tokens::Token> construct(const aux::ir::tokens::Span &span) const;

    private:
        const bool _result;
        const std::string _token;
        const std::shared_ptr<std::runtime_error> _scannerError; // todo: may do without shared pointers
        const ConstructingFunction _constructionFunction;
    };

    template<typename ResultType>
    std::shared_ptr<aux::ir::tokens::Token>
    makeTokenSharedPtr(const std::string &s, const aux::ir::tokens::Span &span) {
        return make_shared<ResultType>(s, span);
    }

}

#endif //AUX_SCANTOKENRESULT_H
