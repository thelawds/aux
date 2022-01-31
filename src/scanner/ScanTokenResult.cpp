//
// Created by miserable on 28.01.2022.
//

#include "ScanTokenResult.h"

#include <utility>

using namespace std;
using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::exception;

std::shared_ptr<Token> defaultConstructionFunction(const CommonStringType &token, const Span &span) {
    return make_shared<TokenUndefined>(span);
}

ScanTokenResult::ScanTokenResult(runtime_error &runtimeError)
        : _result(false),
          _scannerError(make_shared<runtime_error>(runtimeError)),
          _constructionFunction(defaultConstructionFunction) {}


ScanTokenResult::ScanTokenResult(CommonStringType token, ScanTokenResult::ConstructingFunction constructingFunction)
        : _result(true),
          _token(std::move(token)),
          _constructionFunction(constructingFunction),
          _scannerError(nullptr) {}

ScanTokenResult::operator bool() const {
    return _result;
}

CommonStringType ScanTokenResult::getToken() const {
    return _token;
}

std::shared_ptr<std::runtime_error> ScanTokenResult::getScannerError() const {
    return _scannerError;
}

shared_ptr<Token> ScanTokenResult::construct(const Span &span) const {
    return _constructionFunction(_token, span);
}
