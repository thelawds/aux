//
// Created by miserable on 31.01.2022.
//

#include "CommentsScanner.h"

aux::scanner::components::CommentsScanner::CommentsScanner(std::istream &stream) : _stream(stream) {}

aux::scanner::ScanTokenResult aux::scanner::components::CommentsScanner::next(aux::ir::tokens::Span span) const {
    char c1, c2;
    _stream.get(c1).get(c2);

    if (c1 == '-' && c2 == '-') {
        std::string result;
        char curr;

        while (_stream.peek() != std::char_traits<char>::eof()) {
            _stream.get(curr);

            if (curr == '\n') {
                break;
            }

            result += curr;

        }

        return {result, aux::scanner::makeTokenSharedPtr<ir::tokens::TokenComment>};

    } else {
        _stream.unget();
        std::runtime_error err{"Unable to scan comment. Comment Should start with --"};
        return err;
    }


}

bool aux::scanner::components::CommentsScanner::canProcessNextToken() const {
    char curr;
    _stream.get(curr);

    if (curr == '-' && _stream.peek() == '-') {
        _stream.unget();
        return true;
    }

    _stream.unget();
    return false;
}
