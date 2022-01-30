//
// Created by miserable on 31.01.2022.
//

#include "IdentifierAndKeywordScanner.h"
#include "../characters/Delimiters.h"
#include "../characters/IdentifierKeyword.h"

using namespace std;
using namespace aux::fsa;
using namespace aux::scanner;
using namespace aux::exception;
using namespace aux::ir::tokens;
using namespace aux::scanner::characters;
using namespace aux::scanner::components;

IdentifierAndKeywordScanner::IdentifierAndKeywordScanner(istream &stream) : _stream(stream) {
    _startingState = make_shared<BasicFsaState>(stream);
    auto S_Finish = make_shared<BasicFsaFinalState>(stream);

    DeclareIntermediateState(S_IK);

    _startingState->addTransition(identifier(ALPHABETIC_UNDERSCORE), S_IK);

    S_IK->addTransition(delimiter(ANY), S_Finish, SKIP_SYMBOL);
    S_IK->addTransition(identifier(ANY), S_IK);
}

ScanTokenResult IdentifierAndKeywordScanner::next(Span span) const {
    try {
        auto result = _startingState->start();
        if (aux::ir::tokens::isKeyword(result)) {
            return {result, makeTokenSharedPtr<TokenKeyword>};
        } else {
            return {result, makeTokenSharedPtr<TokenIdentifier>};
        }
    } catch (runtime_error &error) {
        return error;
    }
}

bool IdentifierAndKeywordScanner::canProcessNextToken() const {
    auto nextChar = _stream.peek();
    if (static_cast<char>(nextChar) >>= IdentifierKeywordCharType::ALPHABETIC_UNDERSCORE) {
        return true;
    } else {
        return false;
    }
}
