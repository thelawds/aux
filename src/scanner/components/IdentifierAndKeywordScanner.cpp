
#include "IdentifierAndKeywordScanner.h"
#include "scanner/characters/Delimiters.h"
#include "scanner/characters/IdentifierKeyword.h"

using namespace std;
using namespace aux::fsa;
using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::scanner::characters;
using namespace aux::scanner::components;

IdentifierAndKeywordScanner::IdentifierAndKeywordScanner(input_stream::IIndexedStream<char> &stream) : _stream(stream) {
    _startingState = make_shared<BasicFsaState>(stream);
    auto S_FinishNonEOF = make_shared<BasicFsaFinalStateReturningLastCharacter>(stream);
    auto S_FinishEOF = make_shared<BasicFsaFinalState>(stream);

    DeclareIntermediateState(S_IK);

    _startingState->addTransition(identifier(ALPHABETIC_UNDERSCORE), S_IK);

    S_IK->addTransition(delimiter(END_OF_FILE), S_FinishEOF, SKIP_SYMBOL);
    S_IK->addTransition(delimiter(NON_EOF), S_FinishNonEOF, SKIP_SYMBOL);
    S_IK->addTransition(identifier(ANY), S_IK);
}

ScanTokenResult IdentifierAndKeywordScanner::next() const {
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
    char nextChar = _stream.peek();
    if (nextChar >>= IdentifierKeywordCharType::ALPHABETIC_UNDERSCORE) {
        return true;
    } else {
        return false;
    }
}
