//
// Created by miserable on 31.01.2022.
//

#ifndef AUX_MODULARSCANNER_H
#define AUX_MODULARSCANNER_H


#include <vector>
#include <memory>
#include "IScanner.h"
#include "input_stream/IIndexedStream.h"
#include "components/IScannerComponent.h"

namespace aux::scanner {

    struct ModularScanner : IScanner {
        explicit ModularScanner(input_stream::IIndexedStream<CommonCharType> &stream);

        [[nodiscard]]
        std::shared_ptr<ir::tokens::Token> next() const override;

        [[nodiscard]]
        std::shared_ptr<ir::tokens::Token> peek() const override;

    private:
        std::vector<std::unique_ptr<components::IScannerComponent>> _components {};
        input_stream::IIndexedStream<CommonCharType> &_stream;

        mutable std::shared_ptr<ir::tokens::Token> peekToken {nullptr};
        mutable bool hasPeekToken {false};
    };
}


#endif //AUX_MODULARSCANNER_H
