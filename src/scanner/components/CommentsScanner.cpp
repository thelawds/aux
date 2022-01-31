//
// Created by miserable on 31.01.2022.
//

#include "CommentsScanner.h"

aux::scanner::components::CommentsScanner::CommentsScanner(aux::scanner::input_stream::IIndexedStream<CommonCharType> &stream)
        : _stream(stream) {}

aux::scanner::ScanTokenResult aux::scanner::components::CommentsScanner::next() const {
    CommonCharType c1 = _stream.get();
    CommonCharType c2 = _stream.get();

    if (c1 == '-' && c2 == '-') {
        CommonStringType result;
        CommonCharType curr;

        while (_stream.peek() != std::char_traits<CommonCharType>::eof()) {
            curr = _stream.get();

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
    CommonCharType curr = _stream.get();

    if (curr == '-' && _stream.peek() == '-') {
        _stream.unget();
        return true;
    }

    _stream.unget();
    return false;
}
