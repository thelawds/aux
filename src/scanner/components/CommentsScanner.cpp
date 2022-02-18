//
// Created by miserable on 31.01.2022.
//

#include "CommentsScanner.h"

aux::scanner::components::CommentsScanner::CommentsScanner(aux::scanner::input_stream::IIndexedStream<char> &stream)
        : _stream(stream) {}

aux::scanner::ScanTokenResult aux::scanner::components::CommentsScanner::next() const {
        std::string result;

        while (_stream.peek() != std::char_traits<char>::eof()) {
            char curr = _stream.get();

            if (curr == '\n') {
                break;
            }

            result += curr;
        }

        return {result, aux::scanner::makeTokenSharedPtr<ir::tokens::TokenComment>};
}

bool aux::scanner::components::CommentsScanner::canProcessNextToken() const {
    char c1 = _stream.get();
    char c2 = _stream.peek();
    _stream.unget();

    if (c1 == '-' && c2 == '-') {
        return true;
    } else {
        return false;
    }

}
