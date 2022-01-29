//
// Created by miserable on 28.01.2022.
//

#include "ScanTokenResult.h"

using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::exception;

ScanTokenResult::ScanTokenResult(bool result, ScannerError *scannerError, Token *token)
        : _result(result), _scannerError(scannerError), _token(token) {}

ScanTokenResult::operator bool() const {
    return this->_result;
}

ScannerError *ScanTokenResult::getScannerError() const {
    return this->_scannerError;
}

Token *ScanTokenResult::getToken() const {
    return this->_token;
}

ScanTokenResult::ScanTokenResult(ScannerError *scannerError) : ScanTokenResult(false, scannerError, nullptr) {}

ScanTokenResult::ScanTokenResult(Token *token) : ScanTokenResult(true, nullptr, token) {}
