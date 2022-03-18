//
// Created by miserable on 30.01.2022.
//

#include "OperatorScanner.h"
#include "exception/Exception.h"

using namespace std;
using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::exception;
using namespace aux::scanner::components;

ScanTokenResult OperatorScanner::next() const {
    try {
        auto res = tryScanOperatorOrDelimiter();
        return {res, makeTokenSharedPtr<TokenOperator>};
    } catch (std::runtime_error &error) {
        return error;
    }
}

std::string OperatorScanner::tryScanOperatorOrDelimiter() const {
    char curr = 0;

    if (_stream.peek() != char_traits<char>::eof()) {
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
                    return ("//");
                } else {
                    return ("/");
                }
            case '~':
                if (_stream.peek() == '=') {
                    _stream.get();
                    return ("~=");
                } else {
                    return ("~");
                }
            case '<':
                if (_stream.peek() == '<') {
                    _stream.get();
                    return ("<<");
                } else if (_stream.peek() == '=') {
                    _stream.get();
                    return ("<=");
                } else {
                    return ("<");
                }
            case '>':
                if (_stream.peek() == '>') {
                    _stream.get();
                    return (">>");
                } else if (_stream.peek() == '=') {
                    _stream.get();
                    return (">=");
                } else {
                    return (">");
                }
            case '=':
                if (_stream.peek() == '=') {
                    _stream.get();
                    return ("==");
                } else {
                    return ("=");
                }
            case ':':
                if (_stream.peek() == ':') {
                    _stream.get();
                    return ("::");
                } else {
                    return (":");
                }
            case '.':
                if (_stream.peek() == '.') {
                    _stream.get();
                    if (_stream.peek() == '.') {
                        _stream.get();
                        return ("...");
                    } else {
                        return ("..");
                    }
                } else {
                    return (".");
                }
            default:
                _stream.unget();
                throw PatternMatchingException("Pattern matching failed for operator scanning", curr);
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
