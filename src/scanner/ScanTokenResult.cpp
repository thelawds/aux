//
// Created by miserable on 28.01.2022.
//

#include "ScanTokenResult.h"

#include <utility>

using namespace std;
using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::exception;

ScanTokenResult::ScanTokenResult(shared_ptr<exception::ScannerError> scannerError)
        : _result(false), _scannerError(std::move(scannerError)), _token(nullptr) {}

ScanTokenResult::ScanTokenResult(shared_ptr<ir::tokens::Token> token)
        : _result(true), _scannerError(nullptr), _token(std::move(token)) {}

ScanTokenResult::operator bool() const {
    return _result;
}

std::shared_ptr<aux::exception::ScannerError> ScanTokenResult::getScannerError() const {
    return _scannerError;
}

std::shared_ptr<aux::ir::tokens::Token> ScanTokenResult::getToken() const {
    return _token;
}
