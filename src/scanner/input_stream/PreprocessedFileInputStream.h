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
#include "../../util/Defines.h"

namespace aux::scanner::input_stream {

    struct PreprocessedFileInputStream : IIndexedStream<CommonCharType> {

        explicit PreprocessedFileInputStream(const std::string& inputFile);

        CommonCharType get() override;

        CommonCharType peek() override;

        uint16_t getRow() override;

        void unget() override;

        uint16_t getColumn() override;

        std::string skipToTheEndOfCurrRow() override;

    private:
        const std::unique_ptr<std::basic_ifstream<CommonCharType>> _stream;
        CommonCharType _prevReturned{};
        bool _prevReturnSubstituted{false};
        uint16_t row{0}, col{0};
        std::vector<uint16_t> rowSizes;
        std::string currRow;
    };

}


#endif //AUX_PREPROCESSEDFILEINPUTSTREAM_H
