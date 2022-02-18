//
// Created by miserable on 31.01.2022.
//

#include "PreprocessedFileInputStream.h"
#include <iostream>
#include <unordered_set>

aux::scanner::input_stream::PreprocessedFileInputStream::PreprocessedFileInputStream(const std::string &inputFile)
        : _stream(std::make_unique<std::basic_ifstream<char>>(inputFile, std::ios::in)), rows(1) {}

char aux::scanner::input_stream::PreprocessedFileInputStream::peek() {
    return _stream->peek();
}

char aux::scanner::input_stream::PreprocessedFileInputStream::get() {
    auto curr = static_cast<char>(_stream->get());

    if (std::isdigit(_prevReturned) && curr == '.' && _stream->peek() == '.') {
        _stream->unget();
        curr = ' ';
        _prevReturned = curr;
        _prevReturnSubstituted = true;
        return curr;
    }

    if (curr == '\n' || curr == std::char_traits<char>::eof()) {
        rows.emplace_back("");
        rowSizes.push_back(col);
        col = 0;
        ++row;
    } else {
        if (isascii(curr)) {
            ++col;
        } else {
            if (!incrementedOnPrevNonAsciiChar) {
                col++;
                incrementedOnPrevNonAsciiChar = true;
            } else {
                incrementedOnPrevNonAsciiChar = false;
            }
        }
        rows[row].push_back(curr);
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
        bool needRollback = isascii(_stream->peek());
        if (!needRollback) {
            if (incrementedOnPrevNonAsciiChar) {
                needRollback = true;
                incrementedOnPrevNonAsciiChar = false;
            } else {
                incrementedOnPrevNonAsciiChar = true;
            }
        }

        if (needRollback) {
            --col;
            rows[row].pop_back();
        }
    }
}

uint16_t aux::scanner::input_stream::PreprocessedFileInputStream::getRow() {
    return row;
}

uint16_t aux::scanner::input_stream::PreprocessedFileInputStream::getColumn() {
    return col;
}

std::string aux::scanner::input_stream::PreprocessedFileInputStream::skipToTheEndOfCurrRow() {
    static std::unordered_set<char> stop{'\n', std::char_traits<char>::eof()};
    while (!stop.contains(get())) {}
    return rows[row - 1];
}
