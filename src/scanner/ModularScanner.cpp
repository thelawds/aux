//
// Created by miserable on 31.01.2022.
//

#include "ModularScanner.h"

#include "components/CommentsScanner.h"
#include "components/OperatorScanner.h"
#include "components/StringLiteralScanner.h"
#include "components/IdentifierAndKeywordScanner.h"
#include "components/NumericConstantsDFSAScanner.h"
#include "../exception/ErrorMessages.h"

using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::scanner::input_stream;

Span constructSpan(int row, int column) {
    return Span{static_cast<uint16_t>(row), static_cast<uint16_t>(column)};
}

std::shared_ptr<Token> aux::scanner::ModularScanner::next() const {
    if (hasPeekToken) {
        hasPeekToken = false;
        return peekToken;
    }

    if (_stream.peek() == std::char_traits<char>::eof()) {
        return std::make_shared<TokenEofOrUndefined>(constructSpan(_stream.getRow() + 1, _stream.getColumn() + 1));
    }

    while (std::isspace(_stream.peek())) {
        _stream.get();
    }

    char startingChar = _stream.peek();
    Span span = constructSpan(_stream.getRow() + 1, _stream.getColumn() + 1);
    std::vector<std::shared_ptr<std::runtime_error>> errors;
    for (const auto &component: _components) {
        if (component->canProcessNextToken()) {
            auto result = component->next();
            if (result) {
                auto constructed = result.construct(span);
                if (!_returnComments && constructed->getType() == TokenType::COMMENT) {
                    return next();
                }
                return constructed;
            } else {
                errors.push_back(result.getScannerError());
            }
        }
    }

    for (const auto &err: errors) {
        LOG(ERROR) << LA_ERROR_SCANNING_FILE(span.row, span.column, err->what());
    }

    LOG(FATAL) << LA_ERROR_SCANNING_TOKEN(startingChar, span.row, span.column);
}

ModularScanner::ModularScanner(IIndexedStream<char> &stream, bool returnComments)
        : _stream(stream), _returnComments(returnComments) {

    _components.push_back(std::make_unique<components::CommentsScanner>(_stream));
    _components.push_back(std::make_unique<components::OperatorScanner>(_stream));
    _components.push_back(std::make_unique<components::IdentifierAndKeywordScanner>(_stream));
    _components.push_back(std::make_unique<components::StringLiteralScanner>(_stream));
    _components.push_back(std::make_unique<components::NumericConstantsDFSAScanner>(_stream));
}

std::shared_ptr<Token> ModularScanner::peek() const {
    if (hasPeekToken) {
        return peekToken;
    } else {
        this->peekToken = next();
        hasPeekToken = true;
        return peekToken;
    }
}
