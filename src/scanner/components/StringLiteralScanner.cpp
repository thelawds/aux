//
// Created by miserable on 30.01.2022.
//

#include "StringLiteralScanner.h"
#include "../characters/StringLiteral.h"
#include <unordered_set>
#include <memory>

using namespace std;
using namespace aux::fsa;
using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::scanner::components;

char escape(char curr){
    switch (curr) {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        case '\\':
            return curr;
        default:
            throw logic_error(
                    "Escape sequence is not supported for " + std::string {static_cast<char>(curr)}
            );
    }
}

std::string makeCorrectEscapeSequence(char curr){
    return curr == '\'' || curr == '\"'
           ? std::string{curr}
           : std::string{escape(curr)};
}

StringLiteralScanner::StringLiteralScanner(input_stream::IIndexedStream<char> &stream) : _stream(stream) {
    _startingState = make_shared<BasicFsaState>(stream);
    auto S_FINISH = make_shared<BasicFsaFinalState>(stream);

    DeclareIntermediateState(S_DoubleQuote_Any);
    DeclareIntermediateState(S_DoubleQuote_Escape);
    DeclareIntermediateState(S_SingleQuote_Any);
    DeclareIntermediateState(S_SingleQuote_Escape);

    _startingState->addTransition(stringLiteral(DOUBLE_QUOTE), S_DoubleQuote_Any, SKIP_SYMBOL);
    _startingState->addTransition(stringLiteral(SINGLE_QUOTE), S_SingleQuote_Any, SKIP_SYMBOL);

    S_DoubleQuote_Any->addTransition(stringLiteral(DOUBLE_QUOTE), S_FINISH, SKIP_SYMBOL);
    S_DoubleQuote_Any->addTransition(stringLiteral(BACKSLASH), S_DoubleQuote_Escape, SKIP_SYMBOL);
    S_DoubleQuote_Any->addTransition(stringLiteral(NON_EOF), S_DoubleQuote_Any);

    S_DoubleQuote_Escape->addTransition(stringLiteral(NON_EOF), S_DoubleQuote_Any, makeCorrectEscapeSequence);

    S_SingleQuote_Any->addTransition(stringLiteral(SINGLE_QUOTE), S_FINISH, SKIP_SYMBOL);
    S_SingleQuote_Any->addTransition(stringLiteral(BACKSLASH), S_SingleQuote_Escape, SKIP_SYMBOL);
    S_SingleQuote_Any->addTransition(stringLiteral(NON_EOF), S_SingleQuote_Any);

    S_SingleQuote_Escape->addTransition(stringLiteral(NON_EOF), S_SingleQuote_Any, makeCorrectEscapeSequence);
}

ScanTokenResult StringLiteralScanner::next() const {
    if (_stream.peek() == '[') {
        return readWithLongBracket();
    } else {
        try {
            std::string result = _startingState->start();
            return {result, makeTokenSharedPtr<TokenStringLiteral>};
        } catch (std::runtime_error &e) {
            return e;
        }
    }
}

bool StringLiteralScanner::canProcessNextToken() const {
    static unordered_set<char> quotationChars{'\'', '\"'};
    static unordered_set<char> longBracketChars{'[', '='};

    if (quotationChars.contains(_stream.peek())) {
        return true;
    }

    char curr = _stream.get();
    if (curr == '[' && longBracketChars.contains(_stream.peek())) {
        _stream.unget();
        return true;
    }

    _stream.unget();
    return false;
}

ScanTokenResult StringLiteralScanner::readWithLongBracket() const {
    throw std::logic_error("Not Implemented =(");
}
