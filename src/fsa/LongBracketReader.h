//
// Created by miserable on 30.01.2022.
//

#ifndef AUX_LONGBRACKETREADER_H
#define AUX_LONGBRACKETREADER_H

#include <string>
#include "../util/Defines.h"
#include "State.h"

namespace aux::fsa::long_bracket {

    enum class LongBracketType {
        OPENING,
        CLOSING
    };

    struct LongBracket {

        LongBracket() : _type(LongBracketType::OPENING), _level(0) {}

        explicit LongBracket(LongBracketType type) : _type(type), _level(0) {}

        LongBracket(const LongBracketType type, uint16_t level) : _type(type), _level(level) {}

        inline LongBracketType getType() const {
            return _type;
        }

        inline uint16_t getLevel() const {
            return _level;
        }

    private:
        const LongBracketType _type;
        mutable uint16_t _level;
    };

    struct LongBracketReader : State<LongBracket> {
        explicit LongBracketReader(std::basic_istream<char, std::char_traits<char>> &stream) : State(stream) {}

        inline LongBracket start() override {
            unGet();

            uint16_t bracketsCount = 0;
            uint16_t equalsCount = 0;
            char bracketType;
            char curr;

            if (!isEof()) {
                get(&curr);
                if (curr == '[' || curr == ']') {
                    bracketType = curr;
                    ++bracketsCount;
                } else {
                    throw PatternMatchingException(
                            "Exception matching long bracket: long bracket should start with [ or ]."
                    );
                }
            }

            while (!isEof()) {
                get(&curr);
                if (curr == '=') {
                    ++equalsCount;
                } else if (curr == bracketType) {
                    ++bracketsCount;
                    break;
                } else {
                    break;
                }
            }

            if (bracketsCount == 2) {
                return {bracketType == '[' ? LongBracketType::OPENING : LongBracketType::CLOSING, equalsCount};
            } else {
                for (int i = 0; i < equalsCount + bracketsCount - 1; ++i) {
                    unGet();
                }
                throw PatternMatchingException(
                        "Exception matching long bracket. Unexpected character with code: " + std::to_string(curr));
            }

        }
    };

}

#endif //AUX_LONGBRACKETREADER_H
