//
// Created by miserable on 10.02.2022.
//

#ifndef AUX_PARSER_H
#define AUX_PARSER_H

#include <variant>
#include <unordered_set>

#include "../scanner/IScanner.h"
#include "../intermediate_representation/Tree.h"

namespace aux::parser {

    struct Parser {

        explicit Parser(std::shared_ptr<scanner::IScanner> scanner);

        std::shared_ptr<ir::ast::BaseTree> parse();

    private:
        std::shared_ptr<scanner::IScanner> _scanner;

        void checkNextTokenEquals(const std::string &expected, bool isStatement);

        void checkNextTokenIn(const std::unordered_set<std::string> &expected, bool isStatement);

        void checkNextTokenTypeEquals(const ir::tokens::TokenType &expected, bool isStatement);

        std::shared_ptr<ir::tokens::Token> peek();

        std::shared_ptr<ir::tokens::Token> next();

        void skipToken();

        /**
         * block ::= {statement} [returnStatement]
         */
        std::shared_ptr<ir::ast::ListTree> parseBlock();

        /**
         * stat ::= ';' | BREAK | GOTO Identifier | DO block END | whileLoop | ifStatement
	     *        | forLoop| LOCAL (functionDefinition | attributeIdentifierList ['=' expList])
	     *        | label | functionDefinition | assignmentOrFuncCall
         */
        std::shared_ptr<ir::ast::BaseTree> parseStatement();

        /**
         * assignmentOrFuncCall ::= functionCall | assignment
         */
        std::shared_ptr<ir::ast::BinTree> parseAssignmentOrFunctionCall();

        /**
         * ifStatement ::= IF exp THEN block {ELSEIF exp THEN block} [ELSE block] END
         */
        std::shared_ptr<ir::ast::ListTree> parseIfStatement();

        /**
         * whileLoop ::= WHILE exp DO block END | REPEAT block UNTIL exp
         */
        std::shared_ptr<ir::ast::BinTree> parseWhileLoop();

        /**
         * functionDefinition ::= [LOCAL] FUNCTION funcIdentifier funcBody
         */
        std::shared_ptr<ir::ast::BinTree> parseFunctionDefinition();

        /**
         * forLoop ::= FOR IdentifierList ('=' | In) expList DO block END
         */
        std::shared_ptr<ir::ast::ForLoopTree> parseForLoop();

        /**
         * returnStatement ::= RETURN [expLst] [';']
         */
        std::shared_ptr<ir::ast::BinTree> parseReturnStatement();

        /**
         * assignment ::= varList '=' expList
         */
        std::shared_ptr<ir::ast::BinTree> parseAssignment();

        /**
         * funcIdentifier ::= Identifier {'.' Identifier} [':' Identifier]
         */
        std::shared_ptr<ir::ast::ListTree> parseFunctionIdentifier();

        /**
         * funcBody ::= '(' [parList] ')' block END
         */
        std::shared_ptr<ir::ast::BinTree> parseFunctionBody();

        /**
         * label ::= '::' Identifier '::'
         */
        std::shared_ptr<ir::ast::TokenTree> parseLabel();

        /**
         * attIdentifierList ::=  Identifier attrib {',' Identifier attrib}
         */
        std::shared_ptr<ir::ast::ListTree> parseAttribIdentifierList();

        /**
         * parList ::= IdentifierList [',' '...'] | '...'
         */
        std::shared_ptr<ir::ast::BaseTree> parseParList();

        /**
         * IdentifierList ::= Identifier {',' Identifier}
         */
        std::shared_ptr<ir::ast::ListTree> parseIdentifierList(bool parseAdditionalDotDotDot = false);

        /**
         * attrib ::= ['\<' Identifier '>']
         */
        std::shared_ptr<ir::ast::TokenTree> parseAttribute();

        /**
         * expList ::= exp {',', exp}
         */
        std::shared_ptr<ir::ast::ListTree> parseExprList();

        /**
         * exp ::= '...' | FUNCTION funcBody | tableConstructor | arlExpr
         */
        std::shared_ptr<ir::ast::BaseTree> parseExpr();

        /**
         * functionCall ::= prefixExpr funcCallSuffix
         */
        std::shared_ptr<ir::ast::PrefixExprTree> parseFunctionCall();

        /**
         * prefixExpr ::= (Identifier | '(' exp ')') {peSuffix | funcCallSuffix}
         */
        std::shared_ptr<ir::ast::PrefixExprTree> parsePrefixExpr();

        /**
         * varList::= var {',' var}
         */
        std::shared_ptr<ir::ast::ListTree> parseVarList();

        /**
         * var ::= (Identifier | '(' exp ')')  {peSuffix}
         */
        std::shared_ptr<ir::ast::VariableTree> parseVariable();

        /**
         * peSuffix ::= '[' exp ']' | '.' Identifier
         */
        std::shared_ptr<ir::ast::ExprSuffixTree> parsePrefixExprSuffix();

        /**
         * funcCallSuffix ::= [':' Identifier] args
         */
        std::shared_ptr<ir::ast::FunctionCallSuffixTree> parseFuncCallSuffix();

        /**
         * args ::= '(' [expList] ')' | tableConstructor | LiteralString
         */
        std::shared_ptr<ir::ast::ArgsTree> parseArgs();

        /**
         * tableConstructor ::= '{' [fieldList] '}'
         */
        std::shared_ptr<ir::ast::ListTree> parseTableConstructor();

        /**
         * fieldList ::= field {(',' | ';') field} [(',' | ';')]
         */
        std::shared_ptr<ir::ast::ListTree> parseTableFieldList();

        /**
         * field ::= '[' exp ']' '=' exp | Identifier '=' exp | exp
         */
        std::shared_ptr<ir::ast::BinTree> parseTableField();

        /**
         * arlExpression ::= logicalAndTerm {OR logicalOrTerm}
         */
        std::shared_ptr<ir::ast::BinTree> parseArlExpr();

        /**
         * logicalAndTerm ::= relationalTerm {AND relationalTerm}
         */
        std::shared_ptr<ir::ast::BinTree> parseLogicalAndTerm();

        /**
         * relationalTerm     ::= bitwiseOrTerm {RelationalOperator bitwiseOrTerm}
         * RelationalOperator ::= ('\<' | '>' | '\<=' | '>=' | '~=' | '==')
         */
        std::shared_ptr<ir::ast::BinTree> parseRelationalTerm();

        /**
         * bitwiseOrTerm ::= bitwiseXorTerm {'|' bitwiseXorTerm}
         */
        std::shared_ptr<ir::ast::BinTree> parseBitwiseOrTerm();

        /**
         * bitwiseXorTerm ::= bitwiseAndTerm {'~' bitwiseAndTerm}
         */
        std::shared_ptr<ir::ast::BinTree> parseBitwiseXorTerm();

        /**
         * bitwiseAndTerm ::= shiftedTerm {'&' shiftedTerm}
         */
        std::shared_ptr<ir::ast::BinTree> parseBitwiseAndTerm();

        /**
         * shiftedTerm ::= stringConcatenationTerm {('<<' | '>>') stringConcatenationTerm
         */
        std::shared_ptr<ir::ast::BinTree> parseShiftedTerm();

        /**
         * stringConcatenationTerm ::= summationTerm {'..' summationTerm}
         */
        std::shared_ptr<ir::ast::BinTree> parseStringConcatenationTerm();

        /**
         * summationTerm :== productTerm {('+' | '-') productTerm}
         */
        std::shared_ptr<ir::ast::BinTree> parseSummationTerm();

        /**
         * productTerm ::= unaryTerm {('*' | '/' | '//' | '%')  productTerm}
         */
        std::shared_ptr<ir::ast::BinTree> parseProductTerm();

        /**
         * unaryTerm ::= [(NOT | '#' | '-' | '~')] exponentTerm
         */
        std::shared_ptr<ir::ast::BinTree> parseUnaryTerm();

        /**
         * exponentTerm ::= term ['^' exponentTerm]
         */
        std::shared_ptr<ir::ast::BinTree> parseExponentTerm();

        /**
         * term ::= NIL | FALSE | TRUE | Numeral | LiteralString | prefixExpr
         */
        std::shared_ptr<ir::ast::TermTree> parseTerm();

    };

}


#endif //AUX_PARSER_H
