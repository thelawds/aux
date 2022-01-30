//
// Created by miserable on 30.01.2022.
//

#include "OperatorScanner.h"
#include "../../exception/PatternMatchingException.h"

using namespace std;
using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::scanner::components;

#define EOF char_traits<char>::eof()

ScanTokenResult OperatorScanner::next(Span span) const {
    try {
        auto res = tryScanOperatorOrDelimiter();
        return {res, makeTokenSharedPtr<TokenOperator>};
    } catch (std::runtime_error &error) {
        return error;
    }
}

string OperatorScanner::tryScanOperatorOrDelimiter() const {
    char curr;

    if (_stream.peek() != EOF) {
        _stream.get(curr);

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
                    _stream.get(curr);
                    return "//";
                } else {
                    return "/";
                }
            case '~':
                if (_stream.peek() == '=') {
                    _stream.get(curr);
                    return "~=";
                } else {
                    return "~";
                }
            case '<':
                if (_stream.peek() == '<') {
                    _stream.get(curr);
                    return "<<";
                } else if (_stream.peek() == '=') {
                    _stream.get(curr);
                    return "<=";
                } else {
                    return "<";
                }
            case '>':
                if (_stream.peek() == '>') {
                    _stream.get(curr);
                    return ">>";
                } else if (_stream.peek() == '=') {
                    _stream.get(curr);
                    return ">=";
                } else {
                    return ">";
                }
            case '=':
                if (_stream.peek() == '=') {
                    _stream.get(curr);
                    return "==";
                }
            case ':':
                if (_stream.peek() == ':') {
                    _stream.get(curr);
                    return "::";
                } else {
                    return ":";
                }
            case '.':
                if (_stream.peek() == '.') {
                    _stream.get(curr);
                    if (_stream.peek() == '.') {
                        _stream.get(curr);
                        return "...";
                    } else {
                        return "..";
                    }
                } else {
                    return ".";
                }
            default:
                _stream.unget();
                break;
        }

    }

    throw fsa::PatternMatchingException("Error"); // todo

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
