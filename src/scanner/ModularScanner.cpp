//
// Created by miserable on 31.01.2022.
//

#include "ModularScanner.h"

#include "components/CommentsScanner.h"
#include "components/OperatorScanner.h"
#include "components/StringLiteralScanner.h"
#include "components/IdentifierAndKeywordScanner.h"
#include "components/NumericConstantsDFSAScanner.h"

using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::scanner::input_stream;

std::shared_ptr<Token> aux::scanner::ModularScanner::next() const {
    while (std::isspace(_stream.peek())) {
        _stream.get();
    }

    if (_stream.peek() == std::char_traits<CommonCharType>::eof()) {
        return nullptr;
    }

    Span span{
        static_cast<uint16_t>(1 + _stream.getRow()),
        static_cast<uint16_t>(1 + _stream.getColumn())
    };
    std::vector<std::shared_ptr<std::runtime_error>> errors;
    for (const auto &component: _components) {
        if (component->canProcessNextToken()) {
            auto result = component->next();
            if (result) {
                return result.construct(span);
            } else {
                errors.push_back(result.getScannerError());
            }
        }
    }

    // todo: log errors

    throw std::runtime_error("was not able to scan input file");
}

ModularScanner::ModularScanner(IIndexedStream<CommonCharType> &stream) : _stream(stream) {
    _components.push_back(std::make_unique<components::CommentsScanner>(_stream));
    _components.push_back(std::make_unique<components::OperatorScanner>(_stream));
    _components.push_back(std::make_unique<components::IdentifierAndKeywordScanner>(_stream));
    _components.push_back(std::make_unique<components::StringLiteralScanner>(_stream));
    _components.push_back(std::make_unique<components::NumericConstantsDFSAScanner>(_stream));
}
