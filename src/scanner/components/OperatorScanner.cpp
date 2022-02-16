//
// Created by miserable on 30.01.2022.
//

#include "OperatorScanner.h"
#include "../../exception/PatternMatchingException.h"

using namespace std;
using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::scanner::components;

#define EOF char_traits<CommonCharType>::eof()

ScanTokenResult OperatorScanner::next() const {
    try {
        auto res = tryScanOperatorOrDelimiter();
        return {res, makeTokenSharedPtr<TokenOperator>};
    } catch (std::runtime_error &error) {
        return error;
    }
}

CommonStringType OperatorScanner::tryScanOperatorOrDelimiter() const {
    CommonCharType curr = 0;

    if (_stream.peek() != EOF) {
        curr = _stream.get();

        switch (curr) {
            case '+':
            case '-':
            case '*':
            case '%':
            case '^':
            case '#':
            case '&':
            case '|':
            case '(':
            case ')':
            case '{':
            case '}':
            case '[':
            case ']':
            case ';':
            case ',':
                return {curr};
            case '/':
                if (_stream.peek() == '/') {
                    _stream.get();
                    return toCommonStringType("//");
                } else {
                    return toCommonStringType("/");
                }
            case '~':
                if (_stream.peek() == '=') {
                    _stream.get();
                    return toCommonStringType("~=");
                } else {
                    return toCommonStringType("~");
                }
            case '<':
                if (_stream.peek() == '<') {
                    _stream.get();
                    return toCommonStringType("<<");
                } else if (_stream.peek() == '=') {
                    _stream.get();
                    return toCommonStringType("<=");
                } else {
                    return toCommonStringType("<");
                }
            case '>':
                if (_stream.peek() == '>') {
                    _stream.get();
                    return toCommonStringType(">>");
                } else if (_stream.peek() == '=') {
                    _stream.get();
                    return toCommonStringType(">=");
                } else {
                    return toCommonStringType(">");
                }
            case '=':
                if (_stream.peek() == '=') {
                    _stream.get();
                    return toCommonStringType("==");
                } else {
                    return toCommonStringType("=");
                }
            case ':':
                if (_stream.peek() == ':') {
                    _stream.get();
                    return toCommonStringType("::");
                } else {
                    return toCommonStringType(":");
                }
            case '.':
                if (_stream.peek() == '.') {
                    _stream.get();
                    if (_stream.peek() == '.') {
                        _stream.get();
                        return toCommonStringType("...");
                    } else {
                        return toCommonStringType("..");
                    }
                } else {
                    return toCommonStringType(".");
                }
            default:
                _stream.unget();
                throw fsa::PatternMatchingException("Pattern matching failed for operator scanning", curr);
        }

    }

}

bool OperatorScanner::canProcessNextToken() const {
    switch (_stream.peek()) {
        case '+':
        case '-':
        case '*':
        case '%':
        case '^':
        case '#':
        case '&':
        case '|':
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case ';':
        case ',':
        case '/':
        case '~':
        case '<':
        case '>':
        case '=':
        case ':':
        case '.':
            return true;
        default :
            return false;
    }
}
