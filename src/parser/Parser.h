//
// Created by miserable on 10.02.2022.
//

#ifndef AUX_PARSER_H
#define AUX_PARSER_H

#include <variant>

#include "../scanner/IScanner.h"
#include "../intermediate_representation/ast/Tree.h"

namespace aux::parser {

    struct Parser {

        explicit Parser(std::shared_ptr<scanner::IScanner> scanner);

        std::shared_ptr<ir::ast::BaseTree> parse();

    private:
        std::shared_ptr<scanner::IScanner> _scanner;

        void checkNextTokenEquals(const std::string& expected);
        void checkNextTokenTypeEquals(const ir::tokens::TokenType& expected);

        std::shared_ptr<ir::tokens::Token> peek();
        std::shared_ptr<ir::tokens::Token> next();
        void skipToken();

        /**
         * expList ::= exp {',', exp}
         * @return
         */
        std::shared_ptr<ir::ast::ListTree> parseExprList();

        /**
         * exp ::= functionDef | tableConstructor | arlExpr
         * @return
         */
        std::shared_ptr<ir::ast::BaseTree> parseExpr();

        /**
         * functionCall ::= prefixExpr funcCallSuffix
         * @return the corresponding tree
         */
        std::shared_ptr<ir::ast::PrefixExprTree> parseFunctionCall();

        /**
         * prefixExpr ::= (Identifier | '(' exp ')') {peSuffix | funcCallSuffix}
         * @return the corresponding tree
         */
        std::shared_ptr<ir::ast::PrefixExprTree> parsePrefixExpr();

        /**
         * var ::= (Identifier | '(' exp ')')  {peSuffix}
         * @return the corresponding tree
         */
        std::shared_ptr<ir::ast::VariableTree> parseVariable();

        /**
         * peSuffix ::= '[' exp ']' | '.' Identifier
         * @return
         */
        std::shared_ptr<ir::ast::ExprSuffixTree> parsePrefixExprSuffix();

        /**
         * funcCallSuffix ::= [':' Identifier] args
         * @return
         */
        std::shared_ptr<ir::ast::FunctionCallSuffixTree> parseFuncCallSuffix();

        /**
         * TODO: The grammar is incorrect for testing purposes
         * args ::= '(' [expList] ')'
         *        | tableConstructor
         *        | LiteralString
         * @return
         */
        std::shared_ptr<ir::ast::ArgsTree> parseArgs();

        /**
         * tableConstructor ::= '{' [fieldList] '}'
         * @return
         */
        std::shared_ptr<ir::ast::ListTree> parseTableConstructor();

        /**
         * fieldList ::= field {(',' | ';') field} [(',' | ';')]
         * @return
         */
        std::shared_ptr<ir::ast::ListTree> parseTableFieldList();

        /**
         * field ::= '[' exp ']' '=' exp | Identifier '=' exp | exp
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseTableField();

        /**
         * arlExpression ::= logicalAndTerm {OR logicalOrTerm}
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseArlExpr();

        /**
         * logicalAndTerm ::= relationalTerm {AND relationalTerm}
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseLogicalAndTerm();

        /**
         * relationalTerm ::= bitwiseOrTerm {('<' | '>' | '<=' | '>=' | '~=' | '==') bitwiseOrTerm}
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseRelationalTerm();

        /**
         * bitwiseOrTerm ::= bitwiseXorTerm {'|' bitwiseXorTerm}
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseBitwiseOrTerm();

        /**
         * bitwiseXorTerm ::= bitwiseAndTerm {'~' bitwiseAndTerm}
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseBitwiseXorTerm();

        /**
         * bitwiseAndTerm ::= shiftedTerm {'&' shiftedTerm}
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseBitwiseAndTerm();

        /**
         * shiftedTerm ::= stringConcatenationTerm {('<<' | '>>') stringConcatenationTerm
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseShiftedTerm();

        /**
         * stringConcatenationTerm ::= summationTerm {'..' summationTerm}
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseStringConcatenationTerm();

        /**
         * summationTerm :== productTerm {('+' | '-') productTerm}
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseSummationTerm();

        /**
         * productTerm ::= unaryTerm {('*' | '/' | '//' | '%')  productTerm}
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseProductTerm();

        /**
         * unaryTerm ::= [(NOT | '#' | '-' | '~')] exponentTerm
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseUnaryTerm();

        /**
         * exponentTerm ::= term ['^' exponentTerm]
         * @return
         */
        std::shared_ptr<ir::ast::BinTree> parseExponentTerm();

        /**
         * term ::= NIL | FALSE | TRUE | Numeral | LiteralString | prefixExpr
         * @return corresponding termTree
         */
        std::shared_ptr<ir::ast::TermTree> parseTerm();

    };

}


#endif //AUX_PARSER_H
