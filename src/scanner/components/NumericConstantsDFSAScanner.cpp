//
// Created by miserable on 28.01.2022.
//

#include "NumericConstantsDFSAScanner.h"
#include "../characters/Delimiters.h"
#include "../characters/Numeric.h"

using namespace aux::scanner::characters;
using namespace aux::scanner::components;
using namespace aux::ir::tokens;
using namespace aux::exception;
using namespace aux::scanner;
using namespace aux::fsa;
using namespace std;

components::NumericConstantsDFSAScanner::NumericConstantsDFSAScanner(istream &stream) : _stream(stream) {
    _startingState = make_shared<BasicFsaState>(stream);
    auto S_Finish = make_shared<BasicFsaFinalState>(stream);

    // Declare Decimal Number States:
    DeclareIntermediateState(S_0);
    DeclareIntermediateState(S_Dec_Fp);
    DeclareIntermediateState(S_Dec);
    DeclareIntermediateState(S_Dec_Double);
    DeclareIntermediateState(S_Dec_Double_E);
    DeclareIntermediateState(S_Dec_Double_E_Pm);
    DeclareIntermediateState(S_Dec_Double_E_All);

    // Declare Hex Number States:
    DeclareIntermediateState(S_Hex_Begin);
    DeclareIntermediateState(S_Hex_0);
    DeclareIntermediateState(S_Hex);
    DeclareIntermediateState(S_Hex_Double);
    DeclareIntermediateState(S_Hex_Double_P);
    DeclareIntermediateState(S_Hex_Double_P_Pm);
    DeclareIntermediateState(S_Hex_Double_P_All);

    // Starting State Transitions:
    _startingState->addTransition(numeric(ZERO), S_0);
    _startingState->addTransition(numeric(POSITIVE_DECIMAL), S_Dec);
    _startingState->addTransition(numeric(HEX_DECIMAL_FLOATING_POINT), S_Dec_Fp);

    // Decimal State Transitions:
    S_0->addTransition(numeric(HEX_DECIMAL_FLOATING_POINT), S_Dec_Double);
    S_0->addTransition(numeric(HEX_DELIM), S_Hex_Begin);
    S_0->addTransition(delimiter(ANY), S_Finish);

    S_Dec_Fp->addTransition(numeric(POSITIVE_DECIMAL), S_Dec_Double);

    S_Dec->addTransition(numeric(ALL_DECIMAL), S_Dec);
    S_Dec->addTransition(numeric(HEX_DECIMAL_FLOATING_POINT), S_Dec_Double);
    S_Dec->addTransition(numeric(DECIMAL_EXP), S_Dec_Double_E);
    S_Dec->addTransition(delimiter(ANY), S_Finish);

    S_Dec_Double->addTransition(numeric(ALL_DECIMAL), S_Dec_Double);
    S_Dec_Double->addTransition(numeric(DECIMAL_EXP), S_Dec_Double_E);
    S_Dec_Double->addTransition(delimiter(ANY), S_Finish);

    S_Dec_Double_E->addTransition(numeric(PLUS_MINUS), S_Dec_Double_E_Pm);
    S_Dec_Double_E->addTransition(numeric(POSITIVE_DECIMAL), S_Dec_Double_E_All);

    S_Dec_Double_E_Pm->addTransition(numeric(POSITIVE_DECIMAL), S_Dec_Double_E_All);

    S_Dec_Double_E_All->addTransition(numeric(ANY), S_Dec_Double_E_All);
    S_Dec_Double_E_All->addTransition(delimiter(ANY), S_Finish);

    // Hex State Transitions:
    S_Hex_Begin->addTransition(numeric(ZERO), S_Hex_0);
    S_Hex_Begin->addTransition(numeric(POSITIVE_HEX), S_Hex);

    S_Hex_0->addTransition(numeric(HEX_DECIMAL_FLOATING_POINT), S_Hex_Double);
    S_Hex_0->addTransition(delimiter(ANY), S_Finish);

    S_Hex->addTransition(numeric(ALL_HEX), S_Hex);
    S_Hex->addTransition(numeric(HEX_DECIMAL_FLOATING_POINT), S_Hex_Double);
    S_Hex->addTransition(numeric(HEX_EXP), S_Hex_Double_P);
    S_Hex->addTransition(delimiter(ANY), S_Finish);

    S_Hex_Double->addTransition(numeric(ALL_HEX), S_Hex_Double);
    S_Hex_Double->addTransition(numeric(HEX_EXP), S_Hex_Double_P);
    S_Hex_Double->addTransition(delimiter(ANY), S_Finish);

    S_Hex_Double_P->addTransition(numeric(PLUS_MINUS), S_Hex_Double_P_Pm);
    S_Hex_Double_P->addTransition(numeric(POSITIVE_HEX), S_Hex_Double_P_All);

    S_Hex_Double_P_Pm->addTransition(numeric(POSITIVE_HEX), S_Hex_Double_P_All);

    S_Hex_Double_P_All->addTransition(numeric(ALL_HEX), S_Hex_Double_P_All);
    S_Hex_Double_P_All->addTransition(delimiter(ANY), S_Finish);
}

ScanTokenResult components::NumericConstantsDFSAScanner::next(Span span) const {
    try {
        string res = _startingState->start();
        if (!_stream.eof()) {
            res.erase(res.end() - 1);
            _stream.unget();
        }

        bool isHex = false;
        bool isDouble = false;
        for (const char &c: res) {
            isHex |= c >>= NumericCharType::HEX_DELIM;
            isDouble |= (c >>= NumericCharType::HEX_DECIMAL_FLOATING_POINT)
                        || (c >>= NumericCharType::DECIMAL_EXP)
                        || (c >>= NumericCharType::HEX_EXP);

            if (isDouble) {
                break;
            }
        }

        if (isDouble) {
            return {make_shared<TokenDouble>(res, span)};
        } else if (isHex) {
            return {make_shared<TokenHex>(res, span)};
        } else {
            return {make_shared<TokenDecimal>(res, span)};
        }
    } catch (std::runtime_error &err) {
        return {make_shared<ScannerError>(span)};
    }
}

bool components::NumericConstantsDFSAScanner::canProcessNextToken() const {
    char curr;

    if (_stream.peek() != -1) {
        _stream.get(curr);

        if (isdigit(curr) || curr == '.' && isdigit(_stream.peek())) {
            _stream.unget();
            return true;
        }

        _stream.unget();
    }

    return false;
}
