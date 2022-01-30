//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_STATE_H
#define AUX_STATE_H

#include <istream>
#include <map>
#include <memory>
#include "PatternMatchingException.h"
#include "../util/Defines.h"

namespace aux::fsa {

    template<
            typename ResultType, // ResultType += ResultType; ResultType += InputPredicate; ResultType()
            typename CharT = char,
            typename Traits = std::char_traits<CharT>
    >
    struct State {

        using ConformingStateType = State<ResultType, CharT, Traits>;

        State(
                std::basic_istream<CharT, Traits> &stream,
                std::map<Predicate<CharT>, std::shared_ptr<ConformingStateType>> &transitionTable
        ) : _stream(stream), _transitionTable(transitionTable) {}

        explicit State(std::basic_istream<CharT, Traits> &stream) : _stream(stream), _transitionTable({}) {}

        inline bool addTransition(Predicate<CharT> input, std::shared_ptr<ConformingStateType> state) {
            if (_transitionTable.contains(input)) {
                return false;
            } else {
                _transitionTable[input] = state;
                return true;
            }
        }

        inline bool addTransition(
                Predicate<CharT> input,
                std::shared_ptr<ConformingStateType> state,
                Function<CharT, ResultType> mixin
        ) {
            addTransition(input, state);
            if (_mixinTable.contains(input)) {
                return false;
            } else {
                _mixinTable[input] = mixin;
                return true;
            }
        }

        inline bool removeTransition(Predicate<CharT> input) {
            if (_transitionTable.contains(input)) {
                _transitionTable.erase(input);
                return true;
            } else {
                return false;
            }
        }

        inline virtual ResultType start() {
            ResultType result;
            CharT curr;

            get(&curr);
            for (const auto&[matcher, nextState]: _transitionTable) {
                if (matcher(curr)) {

                    if (_mixinTable.contains(matcher)) {
                        result += _mixinTable[matcher](curr);
                    } else {
                        result += curr;
                    }

                    result += nextState->start();
                    return result;
                }
            }

            unGet();
            throw PatternMatchingException("Pattern matching failed on: " + std::to_string(curr));
        }

    protected:
        std::map<Predicate<CharT>, std::shared_ptr<ConformingStateType>> _transitionTable;
        std::map<Predicate<CharT>, Function<CharT, ResultType>> _mixinTable;
        std::basic_istream<CharT, Traits> &_stream;

        inline bool isEof() {
            return _stream.peek() == Traits::eof();
        }

        inline void get(CharT *curr) {
            if (isEof()) {
                *curr = Traits::eof();
            } else {
                _stream.get(*curr);
            }
        }

        inline void unGet() {
            _stream.unget();
        }

    };

    template<
            typename ResultType, // ResultType += ResultType; ResultType += InputPredicate
            typename CharT = char,
            typename Traits = std::char_traits<CharT>
    >
    struct FinalState : State<ResultType, CharT, Traits> {
        explicit FinalState(std::basic_istream<CharT, Traits> &stream) : State<ResultType, CharT, Traits>(stream) {}

        ResultType start() override {
            return ResultType{};
        }
    };

    using BasicFsaState = State<std::string>;
    using BasicFsaFinalState = FinalState<std::string>;

    inline std::string skipSymbol(char c){
        return {};
    }
}

#define DeclareIntermediateState(_STATE_NAME) auto (_STATE_NAME) = std::make_shared<aux::fsa::BasicFsaState>(_stream)
#define SKIP_SYMBOL aux::fsa::skipSymbol

#endif //AUX_STATE_H
