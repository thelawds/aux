//
// Created by miserable on 31.01.2022.
//

#ifndef AUX_PREPROCESSEDFILEINPUTSTREAM_H
#define AUX_PREPROCESSEDFILEINPUTSTREAM_H

#include <istream>
#include <fstream>
#include <memory>
#include <vector>
#include "IIndexedStream.h"

namespace aux::scanner::input_stream {

    struct PreprocessedFileInputStream : IIndexedStream<char> {

        explicit PreprocessedFileInputStream(const std::string& inputFile);

        char get() override;

        char peek() override;

        uint16_t getRow() override;

        void unget() override;

        uint16_t getColumn() override;

        std::string skipToTheEndOfCurrRow() override;

    private:
        const std::unique_ptr<std::basic_ifstream<char>> _stream;

        char _prevReturned{};
        bool _prevReturnSubstituted{false};
        bool incrementedOnPrevNonAsciiChar{false};

        uint16_t row{0}, col{0};
        std::vector<uint16_t> rowSizes;
        std::vector<std::string> rows;
    };

}


#endif //AUX_PREPROCESSEDFILEINPUTSTREAM_H
