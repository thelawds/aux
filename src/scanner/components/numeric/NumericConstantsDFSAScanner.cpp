//
// Created by miserable on 28.01.2022.
//

#include "NumericConstantsDFSAScanner.h"

using namespace aux::scanner::components::characters;
using namespace aux::scanner::components::numeric;
using namespace aux::ir::tokens;
using namespace aux::exception;
using namespace aux::scanner;
using namespace aux::fsa;
using namespace std;

using BoolPredicate = bool (*)(char);

using NumericFsaState = State<string>;
using NumericFsaFinalState = FinalState<string>;

bool isZero(char c) {
    return c == '0';
}

bool isHexDelim(char c){
    return c == 'x' || c == 'X';
}

bool isAllHex(char c){
    return isdigit(c) || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
}

bool isPlusOrMinus(char c){
    return c == '+' || c == '-';
}

NumericConstantsDFSAScanner::NumericConstantsDFSAScanner(istream &stream) : _stream(stream) {
    _startingState = make_shared<NumericFsaState>(stream);
    auto NHex = make_shared<NumericFsaState>(stream);
    auto NHexX = make_shared<NumericFsaState>(stream);
    auto Final = make_shared<NumericFsaFinalState>(stream);

    _startingState->addState(&isZero, NHex);
    NHex->addState(&isHexDelim, NHexX);
    NHexX->addState(&isAllHex, NHexX);
    NHexX->addState(&isPlusOrMinus, Final);

}

ScanTokenResult NumericConstantsDFSAScanner::next() const {
    try {
        string res = _startingState->start();
        res.erase(res.end() - 1);
        _stream.unget();
        return {true, nullptr, new TokenInteger(stoll(res, nullptr, 16), {1, 1})};
    } catch (std::runtime_error& err){
        return {
                false,
                new ScannerError({1, 1}),
                nullptr
        };
    }
}
