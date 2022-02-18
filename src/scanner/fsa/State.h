//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_STATE_H
#define AUX_STATE_H

#include <istream>
#include <map>
#include <memory>
#include <functional>
#include "../../exception/Exception.h"
#include "../input_stream/IIndexedStream.h"

namespace aux::fsa {

    template<typename IN, typename OUT>
    using Function = OUT (*)(IN);

    template<typename InputType>
    using Predicate = Function<InputType, bool>;

    template<
            typename ResultType, // ResultType += ResultType; ResultType += InputPredicate; ResultType()
            typename CharT = char,
            typename Traits = std::char_traits<CharT>
    >
    struct State {

        using ConformingStateType = State<ResultType, CharT, Traits>;

        State(
                scanner::input_stream::IIndexedStream<CharT, Traits> &stream,
                std::map<Predicate<CharT>, std::shared_ptr<ConformingStateType>> &transitionTable
        ) : _stream(stream), _transitionTable(transitionTable) {}

        explicit State(scanner::input_stream::IIndexedStream<CharT, Traits> &stream)
                : _stream(stream), _transitionTable({}) {}

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
            throw exception::PatternMatchingException("Pattern matching failed", curr);
        }

    protected:
        // todo: should the pointer to the next state be weak_ptr instead of shared_ptr?
        // todo: maybe use bare pointers and RAII
        // Seems there are cyclic dependencies are here
        std::map<Predicate<CharT>, std::shared_ptr<ConformingStateType>> _transitionTable;
        std::map<Predicate<CharT>, Function<CharT, ResultType>> _mixinTable;
        scanner::input_stream::IIndexedStream<CharT, Traits> &_stream;

        inline bool isEof() {
            return _stream.peek() == Traits::eof();
        }

        inline void get(CharT *curr) {
            if (isEof()) {
                *curr = Traits::eof();
            } else {
                *curr = _stream.get();
            }
        }

        inline void unGet() {
            _stream.unget();
        }

    };

    template<
            bool NeedStackRollBack,
            typename ResultType, // ResultType += ResultType; ResultType += InputPredicate
            typename CharT = char,
            typename Traits = std::char_traits<CharT>
    >
    struct FinalState : State<ResultType, CharT, Traits> {
        explicit FinalState(scanner::input_stream::IIndexedStream<CharT, Traits> &stream)
                : State<ResultType, CharT, Traits>(stream) {}

        ResultType start() override {
            if (NeedStackRollBack) {
                this->unGet();
            }
            return ResultType{};
        }
    };

    using BasicFsaState = State<std::string>;
    using BasicFsaFinalStateReturningLastCharacter = FinalState<true, std::string>;
    using BasicFsaFinalState = FinalState<false, std::string>;

    inline std::string skipSymbol(char c){
        return {};
    }
}

#define DeclareIntermediateState(_STATE_NAME) auto (_STATE_NAME) = std::make_shared<aux::fsa::BasicFsaState>(_stream)
#define SKIP_SYMBOL aux::fsa::skipSymbol

#endif //AUX_STATE_H
