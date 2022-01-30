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
using namespace aux::exception;

StringLiteralScanner::StringLiteralScanner(std::istream &stream) : _stream(stream) {
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

    S_DoubleQuote_Escape->addTransition(stringLiteral(NON_EOF), S_DoubleQuote_Any);

    S_SingleQuote_Any->addTransition(stringLiteral(SINGLE_QUOTE), S_FINISH, SKIP_SYMBOL);
    S_SingleQuote_Any->addTransition(stringLiteral(BACKSLASH), S_SingleQuote_Escape, SKIP_SYMBOL);
    S_SingleQuote_Any->addTransition(stringLiteral(NON_EOF), S_SingleQuote_Any);

    S_SingleQuote_Escape->addTransition(stringLiteral(NON_EOF), S_SingleQuote_Any);
}

ScanTokenResult StringLiteralScanner::next(Span span) const {
    if (_stream.peek() == '[') {
        return readWithLongBracket(span);
    } else {
        try {
            std::string result = _startingState->start();
            return {make_shared<TokenStringLiteral>(result, span)};
        } catch (std::runtime_error &e) {
            return {make_shared<ScannerError>(span)};
        }
    }
}

bool StringLiteralScanner::canProcessNextToken() const {
    using istream_char_t = std::basic_istream<char>::int_type;
    static unordered_set<istream_char_t> quotationChars{'\'', '\"'};
    static unordered_set<istream_char_t> longBracketChars{'[', '='};

    if (quotationChars.contains(_stream.peek())) {
        return true;
    }

    istream_char_t curr = _stream.get();
    if (curr == '[' && longBracketChars.contains(_stream.peek())) {
        _stream.unget();
        return true;
    }

    _stream.unget();
    return false;
}

ScanTokenResult StringLiteralScanner::readWithLongBracket(Span span) const {
    throw std::logic_error("Not Implemented =(");
}
