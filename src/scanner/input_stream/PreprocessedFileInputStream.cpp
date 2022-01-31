//
// Created by miserable on 31.01.2022.
//

#include <iostream>
#include "PreprocessedFileInputStream.h"

aux::scanner::input_stream::PreprocessedFileInputStream::PreprocessedFileInputStream(const std::string& inputFile)
        : _stream(std::make_unique<std::basic_ifstream<CommonCharType>>(inputFile)) {}

CommonCharType aux::scanner::input_stream::PreprocessedFileInputStream::peek() {
    return static_cast<CommonCharType>(_stream->peek());
}

CommonCharType aux::scanner::input_stream::PreprocessedFileInputStream::get() {
    auto curr = static_cast<CommonCharType>(_stream->get());

    if (std::isdigit(_prevReturned) && curr == '.' && _stream->peek() == '.') {
        _stream->unget();
        curr = ' ';
        _prevReturned = curr;
        _prevReturnSubstituted = true;
        return curr;
    }

    if (curr == '\n') {
        rowSizes.push_back(col);
        col = 0;
        ++row;
    } else {
        ++col;
    }

    _prevReturned = curr;
    return curr;
}

void aux::scanner::input_stream::PreprocessedFileInputStream::unget() {
    if (row == 0 && col == 0) {
        return;
    }

    if (_prevReturnSubstituted) {
        _prevReturnSubstituted = false;
        return;
    }

    _stream->unget();

    if (col == 0) {
        col = rowSizes[row - 1];
        --row;
    } else {
        --col;
    }
}

uint16_t aux::scanner::input_stream::PreprocessedFileInputStream::getRow() {
    return row;
}

uint16_t aux::scanner::input_stream::PreprocessedFileInputStream::getColumn() {
    return col;
}
