//
// Created by miserable on 10.02.2022.
//

#ifndef AUX_PARSER_H
#define AUX_PARSER_H

#include <variant>
#include <unordered_set>

#include "scanner/IScanner.h"
#include "intermediate_representation/tree/SyntaxTree.h"

namespace aux::parser {

    struct Parser {

        explicit Parser(std::shared_ptr<scanner::IScanner> scanner);

        std::shared_ptr<aux::ir::tree::ProgramTree> parse();

    private:
        std::shared_ptr<scanner::IScanner> _scanner;

        void checkNextTokenEquals(const std::string &expected, bool isStatement);

        void checkNextTokenIn(const std::unordered_set<std::string> &expected, bool isStatement);

        void checkNextTokenTypeEquals(const ir::tokens::TokenType &expected, bool isStatement);

        static void throwInvalidBinaryExpressionException(const std::shared_ptr<ir::tokens::TokenOperator> &op);

        static void throwInvalidBinaryExpressionException(const std::shared_ptr<ir::tokens::TokenKeyword> &keyword);

        std::shared_ptr<ir::tokens::Token> peek();

        std::shared_ptr<ir::tokens::Token> next();

        void skipToken();

        /**
         * block ::= {statement} [returnStatement]
         */
        std::shared_ptr<aux::ir::tree::ProgramTree> parseBlock();

        /**
         * stat ::= ';' | BREAK | GOTO Identifier | DO block END | whileLoop | ifStatement
	     *        | forLoop| LOCAL (functionDefinition | attributeIdentifierList ['=' expList])
	     *        | label | functionDefinition | assignmentOrFuncCall
         */
        std::shared_ptr<aux::ir::tree::StatementTree> parseStatement();

        /**
         * assignmentOrFuncCall ::= functionCall | assignment
         * assignment ::= varList '=' expList
         */
        std::shared_ptr<aux::ir::tree::StatementTree> parseAssignmentOrFunctionCall();

        /**
         * ifStatement ::= IF exp THEN block {ELSEIF exp THEN block} [ELSE block] END
         */
        std::shared_ptr<aux::ir::tree::IfThenElseStatementTree> parseIfStatement();

        /**
         * whileLoop ::= WHILE exp DO block END | REPEAT block UNTIL exp
         */
        std::shared_ptr<aux::ir::tree::WhileLoopTree> parseWhileLoop();

        /**
         * functionDefinition ::= [LOCAL] FUNCTION funcIdentifier funcBody
         */
        std::shared_ptr<aux::ir::tree::FunctionDefinitionTree> parseFunctionDefinition();

        /**
         * forLoop ::= FOR IdentifierList ('=' | In) expList DO block END
         */
        std::shared_ptr<aux::ir::tree::ForLoopStatementTree> parseForLoop();

        /**
         * returnStatement ::= RETURN [expLst] [';']
         */
        std::shared_ptr<aux::ir::tree::ReturnStatementTree> parseReturnStatement();

        /**
         * funcIdentifier ::= Identifier {'.' Identifier} [':' Identifier]
         */
        std::shared_ptr<aux::ir::tree::FunctionIdentifierTree> parseFunctionIdentifier();

        /**
         * funcBody ::= '(' [parList] ')' block END
         */
        std::shared_ptr<aux::ir::tree::FunctionBodyTree> parseFunctionBody();

        /**
         * label ::= '::' Identifier '::'
         */
        std::shared_ptr<aux::ir::tree::LabelTree> parseLabel();

        /**
         * attIdentifierList ::=  Identifier attrib {',' Identifier attrib}
         */
        std::shared_ptr<aux::ir::tree::AttributeIdentifierListTree> parseAttribIdentifierList();

        /**
         * parList ::= IdentifierList [',' '...'] | '...'
         */
        std::shared_ptr<aux::ir::tree::ParameterListTree> parseParList();

        /**
         * IdentifierList ::= Identifier {',' Identifier}
         */
        std::shared_ptr<aux::ir::tree::IdentifierTermListTree> parseIdentifierList(bool allowTrailingComma = false);

        /**
         * attrib ::= ['\<' Identifier '>']
         */
        std::shared_ptr<aux::ir::tree::AttributeTree> parseAttribute();

        /**
         * expList ::= exp {',', exp}
         */
        std::shared_ptr<aux::ir::tree::ExpressionListTree> parseExprList();

        /**
         * exp ::= '...' | FUNCTION funcBody | tableConstructor | arlExpr
         */
        std::shared_ptr<ir::tree::ExpressionTree> parseExpr();

        /**
         * prefixExpr ::= (Identifier | '(' exp ')') {peSuffix | funcCallSuffix}
         */
        std::shared_ptr<aux::ir::tree::PrefixExpressionTermTree> parsePrefixExpr();

        /**
         * varList::= var {',' var}
         */
        std::shared_ptr<aux::ir::tree::VariableListTree> parseVarList();

        /**
         * var ::= (Identifier | '(' exp ')')  {peSuffix}
         */
        std::shared_ptr<aux::ir::tree::VariableTree> parseVariable();

        /**
         * peSuffix ::= '[' exp ']' | '.' Identifier
         */
        std::shared_ptr<aux::ir::tree::PrefixExpressionSuffixTree> parsePrefixExprSuffix();

        /**
         * funcCallSuffix ::= [':' Identifier] args
         */
        std::shared_ptr<aux::ir::tree::FunctionCallSuffixTree> parseFuncCallSuffix();

        /**
         * args ::= '(' [expList] ')' | tableConstructor | LiteralString
         */
        std::shared_ptr<aux::ir::tree::ArgumentsTree> parseArgs();

        /**
         * tableConstructor ::= '{' [fieldList] '}'
         */
        std::shared_ptr<aux::ir::tree::TableConstructorTermTree> parseTableConstructor();

        /**
         * fieldList ::= field {(',' | ';') field} [(',' | ';')]
         */
        std::shared_ptr<aux::ir::tree::TableConstructorTermTree> parseTableFieldList();

        /**
         * field ::= '[' exp ']' '=' exp | Identifier '=' exp | exp
         */
        std::shared_ptr<aux::ir::tree::TableFieldTermTree> parseTableField();

        /**
         * arlExpression ::= logicalAndTerm {OR logicalOrTerm}
         */
        std::shared_ptr<aux::ir::tree::TermTree> parseArlExpr();

        /**
         * logicalAndTerm ::= relationalTerm {AND relationalTerm}
         */
        std::shared_ptr<aux::ir::tree::TermTree> parseLogicalAndTerm();

        /**
         * relationalTerm     ::= bitwiseOrTerm {RelationalOperator bitwiseOrTerm}
         * RelationalOperator ::= ('\<' | '>' | '\<=' | '>=' | '~=' | '==')
         */
        std::shared_ptr<aux::ir::tree::TermTree> parseRelationalTerm();

        /**
         * bitwiseOrTerm ::= bitwiseXorTerm {'|' bitwiseXorTerm}
         */
        std::shared_ptr<aux::ir::tree::TermTree> parseBitwiseOrTerm();

        /**
         * bitwiseXorTerm ::= bitwiseAndTerm {'~' bitwiseAndTerm}
         */
        std::shared_ptr<aux::ir::tree::TermTree> parseBitwiseXorTerm();

        /**
         * bitwiseAndTerm ::= shiftedTerm {'&' shiftedTerm}
         */
        std::shared_ptr<aux::ir::tree::TermTree> parseBitwiseAndTerm();

        /**
         * shiftedTerm ::= stringConcatenationTerm {('<<' | '>>') stringConcatenationTerm
         */
        std::shared_ptr<aux::ir::tree::TermTree> parseShiftedTerm();

        /**
         * stringConcatenationTerm ::= summationTerm {'..' summationTerm}
         */
        std::shared_ptr<aux::ir::tree::TermTree> parseStringConcatenationTerm();

        /**
         * summationTerm :== productTerm {('+' | '-') productTerm}
         */
        std::shared_ptr<aux::ir::tree::TermTree> parseSummationTerm();

        /**
         * productTerm ::= unaryTerm {('*' | '/' | '//' | '%')  productTerm}
         */
        std::shared_ptr<ir::tree::TermTree> parseProductTerm();

        /**
         * unaryTerm ::= [(NOT | '#' | '-' | '~')] exponentTerm
         */
        std::shared_ptr<ir::tree::TermTree> parseUnaryTerm();

        /**
         * exponentTerm ::= term ['^' exponentTerm]
         */
        std::shared_ptr<ir::tree::TermTree> parseExponentTerm();

        /**
         * term ::= NIL | FALSE | TRUE | Numeral | LiteralString | prefixExpr
         */
        std::shared_ptr<ir::tree::TermTree> parseTerm();

    };

}


#endif //AUX_PARSER_H
