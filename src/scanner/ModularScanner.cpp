//
// Created by miserable on 31.01.2022.
//

#include "ModularScanner.h"

#include "components/CommentsScanner.h"
#include "components/OperatorScanner.h"
#include "components/StringLiteralScanner.h"
#include "components/IdentifierAndKeywordScanner.h"
#include "components/NumericConstantsDFSAScanner.h"
#include "../constants/LexicalAnalysisErrorMessages.h"

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

    auto startingChar = _stream.peek();
    Span span{uint16_t(1 + _stream.getRow()), uint16_t(1 + _stream.getColumn())};
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

    for (const auto& err: errors) {
        LOG(ERROR) << MODULAR_SCANNER_ERROR(span.row, span.column, err->what());
    }

    LOG(FATAL) << MODULAR_SCANNER_GENERAL_ERROR(startingChar, span.row, span.column);
}

ModularScanner::ModularScanner(IIndexedStream<CommonCharType> &stream) : _stream(stream) {
    _components.push_back(std::make_unique<components::CommentsScanner>(_stream));
    _components.push_back(std::make_unique<components::OperatorScanner>(_stream));
    _components.push_back(std::make_unique<components::IdentifierAndKeywordScanner>(_stream));
    _components.push_back(std::make_unique<components::StringLiteralScanner>(_stream));
    _components.push_back(std::make_unique<components::NumericConstantsDFSAScanner>(_stream));
}