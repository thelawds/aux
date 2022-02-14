//
// Created by miserable on 31.01.2022.
//

#ifndef AUX_IINDEXEDSTREAM_H
#define AUX_IINDEXEDSTREAM_H

#include <istream>

namespace aux::scanner::input_stream {

    template<typename CharType, typename Traits = std::char_traits<CharType>>
    struct IIndexedStream {

        virtual CharType get() = 0;

        virtual CharType peek() = 0;

        virtual void unget() = 0;

        virtual uint16_t getRow() = 0;

        virtual uint16_t getColumn() = 0;

        virtual std::string skipToTheEndOfCurrRow() = 0;

    };

}

#endif //AUX_IINDEXEDSTREAM_H
