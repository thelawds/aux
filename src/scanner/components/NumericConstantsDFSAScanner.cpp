//
// Created by miserable on 28.01.2022.
//

#include "NumericConstantsDFSAScanner.h"
#include "../characters/Delimiters.h"
#include "../characters/Numeric.h"

using namespace std;
using namespace aux::fsa;
using namespace aux::scanner;
using namespace aux::scanner::input_stream;
using namespace aux::ir::tokens;
using namespace aux::scanner::characters;
using namespace aux::scanner::components;

components::NumericConstantsDFSAScanner::NumericConstantsDFSAScanner(IIndexedStream<char> &stream) : _stream(stream) {
    _startingState = make_shared<BasicFsaState>(stream);
    auto S_FinishNonEof = make_shared<BasicFsaFinalStateReturningLastCharacter>(stream);
    auto S_FinishEof = make_shared<BasicFsaFinalState>(stream);

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
    S_0->addTransition(delimiter(NON_EOF), S_FinishNonEof, SKIP_SYMBOL);
    S_0->addTransition(delimiter(END_OF_FILE), S_FinishEof, SKIP_SYMBOL);

    S_Dec_Fp->addTransition(numeric(POSITIVE_DECIMAL), S_Dec_Double);

    S_Dec->addTransition(numeric(ALL_DECIMAL), S_Dec);
    S_Dec->addTransition(numeric(HEX_DECIMAL_FLOATING_POINT), S_Dec_Double);
    S_Dec->addTransition(numeric(DECIMAL_EXP), S_Dec_Double_E);
    S_Dec->addTransition(delimiter(NON_EOF), S_FinishNonEof, SKIP_SYMBOL);
    S_Dec->addTransition(delimiter(END_OF_FILE), S_FinishEof, SKIP_SYMBOL);

    S_Dec_Double->addTransition(numeric(ALL_DECIMAL), S_Dec_Double);
    S_Dec_Double->addTransition(numeric(DECIMAL_EXP), S_Dec_Double_E);
    S_Dec_Double->addTransition(delimiter(NON_EOF), S_FinishNonEof, SKIP_SYMBOL);
    S_Dec_Double->addTransition(delimiter(END_OF_FILE), S_FinishEof, SKIP_SYMBOL);

    S_Dec_Double_E->addTransition(numeric(PLUS_MINUS), S_Dec_Double_E_Pm);
    S_Dec_Double_E->addTransition(numeric(POSITIVE_DECIMAL), S_Dec_Double_E_All);

    S_Dec_Double_E_Pm->addTransition(numeric(POSITIVE_DECIMAL), S_Dec_Double_E_All);

    S_Dec_Double_E_All->addTransition(numeric(ANY), S_Dec_Double_E_All);
    S_Dec_Double_E_All->addTransition(delimiter(NON_EOF), S_FinishNonEof, SKIP_SYMBOL);
    S_Dec_Double_E_All->addTransition(delimiter(END_OF_FILE), S_FinishEof, SKIP_SYMBOL);

    // Hex State Transitions:
    S_Hex_Begin->addTransition(numeric(ZERO), S_Hex_0);
    S_Hex_Begin->addTransition(numeric(POSITIVE_HEX), S_Hex);

    S_Hex_0->addTransition(numeric(HEX_DECIMAL_FLOATING_POINT), S_Hex_Double);
    S_Hex_0->addTransition(delimiter(NON_EOF), S_FinishNonEof, SKIP_SYMBOL);
    S_Hex_0->addTransition(delimiter(END_OF_FILE), S_FinishEof, SKIP_SYMBOL);

    S_Hex->addTransition(numeric(ALL_HEX), S_Hex);
    S_Hex->addTransition(numeric(HEX_DECIMAL_FLOATING_POINT), S_Hex_Double);
    S_Hex->addTransition(numeric(HEX_EXP), S_Hex_Double_P);
    S_Hex->addTransition(delimiter(NON_EOF), S_FinishNonEof, SKIP_SYMBOL);
    S_Hex->addTransition(delimiter(END_OF_FILE), S_FinishEof, SKIP_SYMBOL);

    S_Hex_Double->addTransition(numeric(ALL_HEX), S_Hex_Double);
    S_Hex_Double->addTransition(numeric(HEX_EXP), S_Hex_Double_P);
    S_Hex_Double->addTransition(delimiter(NON_EOF), S_FinishNonEof, SKIP_SYMBOL);
    S_Hex_Double->addTransition(delimiter(END_OF_FILE), S_FinishEof, SKIP_SYMBOL);

    S_Hex_Double_P->addTransition(numeric(PLUS_MINUS), S_Hex_Double_P_Pm);
    S_Hex_Double_P->addTransition(numeric(POSITIVE_HEX), S_Hex_Double_P_All);

    S_Hex_Double_P_Pm->addTransition(numeric(POSITIVE_HEX), S_Hex_Double_P_All);

    S_Hex_Double_P_All->addTransition(numeric(ALL_HEX), S_Hex_Double_P_All);
    S_Hex_Double_P_All->addTransition(delimiter(NON_EOF), S_FinishNonEof, SKIP_SYMBOL);
    S_Hex_Double_P_All->addTransition(delimiter(END_OF_FILE), S_FinishEof, SKIP_SYMBOL);
}

ScanTokenResult components::NumericConstantsDFSAScanner::next() const {
    try {
        string res = _startingState->start();

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
            return {res, makeTokenSharedPtr<TokenDouble>};
        } else if (isHex) {
            return {res, makeTokenSharedPtr<TokenHex>};
        } else {
            return {res, makeTokenSharedPtr<TokenDecimal>};
        }
    } catch (std::runtime_error &err) {
        return err;
    }
}

bool components::NumericConstantsDFSAScanner::canProcessNextToken() const {
    char curr;

    if (_stream.peek() != char_traits<char>::eof()) {
        curr = _stream.get();

        if (isdigit(curr) || curr == '.' && isdigit(_stream.peek())) {
            _stream.unget();
            return true;
        }

        _stream.unget();
    }

    return false;
}
