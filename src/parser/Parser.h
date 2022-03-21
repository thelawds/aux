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

        std::shared_ptr<aux::ir::syntax_tree::ProgramTree> parse();

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
        std::shared_ptr<aux::ir::syntax_tree::ProgramTree> parseBlock();

        /**
         * stat ::= ';' | BREAK | GOTO Identifier | DO block END | whileLoop | ifStatement
	     *        | forLoop| LOCAL (functionDefinition | attributeIdentifierList ['=' expList])
	     *        | label | functionDefinition | assignmentOrFuncCall
         */
        std::shared_ptr<aux::ir::syntax_tree::StatementTree> parseStatement();

        /**
         * assignmentOrFuncCall ::= functionCall | assignment
         * assignment ::= varList '=' expList
         */
        std::shared_ptr<aux::ir::syntax_tree::StatementTree> parseAssignmentOrFunctionCall();

        /**
         * ifStatement ::= IF exp THEN block {ELSEIF exp THEN block} [ELSE block] END
         */
        std::shared_ptr<aux::ir::syntax_tree::IfThenElseStatementTree> parseIfStatement();

        /**
         * whileLoop ::= WHILE exp DO block END | REPEAT block UNTIL exp
         */
        std::shared_ptr<aux::ir::syntax_tree::WhileLoopTree> parseWhileLoop();

        /**
         * functionDefinition ::= [LOCAL] FUNCTION funcIdentifier funcBody
         */
        std::shared_ptr<aux::ir::syntax_tree::FunctionDefinitionTree> parseFunctionDefinition();

        /**
         * forLoop ::= FOR IdentifierList ('=' | In) expList DO block END
         */
        std::shared_ptr<aux::ir::syntax_tree::ForLoopStatementTree> parseForLoop();

        /**
         * returnStatement ::= RETURN [expLst] [';']
         */
        std::shared_ptr<aux::ir::syntax_tree::ReturnStatementTree> parseReturnStatement();

        /**
         * funcIdentifier ::= Identifier {'.' Identifier} [':' Identifier]
         */
        std::shared_ptr<aux::ir::syntax_tree::FunctionIdentifierTree> parseFunctionIdentifier();

        /**
         * funcBody ::= '(' [parList] ')' block END
         */
        std::shared_ptr<aux::ir::syntax_tree::FunctionBodyTree> parseFunctionBody();

        /**
         * label ::= '::' Identifier '::'
         */
        std::shared_ptr<aux::ir::syntax_tree::LabelTree> parseLabel();

        /**
         * attIdentifierList ::=  Identifier attrib {',' Identifier attrib}
         */
        std::shared_ptr<aux::ir::syntax_tree::AttributeIdentifierListTree> parseAttribIdentifierList();

        /**
         * parList ::= IdentifierList [',' '...'] | '...'
         */
        std::shared_ptr<aux::ir::syntax_tree::ParameterListTree> parseParList();

        /**
         * IdentifierList ::= Identifier {',' Identifier}
         */
        std::shared_ptr<aux::ir::syntax_tree::IdentifierTermListTree> parseIdentifierList(bool allowTrailingComma = false);

        /**
         * attrib ::= ['\<' Identifier '>']
         */
        std::shared_ptr<aux::ir::syntax_tree::AttributeTree> parseAttribute();

        /**
         * expList ::= exp {',', exp}
         */
        std::shared_ptr<aux::ir::syntax_tree::ExpressionListTree> parseExprList();

        /**
         * exp ::= '...' | FUNCTION funcBody | tableConstructor | arlExpr
         */
        std::shared_ptr<ir::syntax_tree::ExpressionTree> parseExpr();

        /**
         * prefixExpr ::= (Identifier | '(' exp ')') {peSuffix | funcCallSuffix}
         */
        std::shared_ptr<aux::ir::syntax_tree::PrefixExpressionTermTree> parsePrefixExpr();

        /**
         * varList::= var {',' var}
         */
        std::shared_ptr<aux::ir::syntax_tree::VariableListTree> parseVarList();

        /**
         * var ::= (Identifier | '(' exp ')')  {peSuffix}
         */
        std::shared_ptr<aux::ir::syntax_tree::VariableTree> parseVariable();

        /**
         * peSuffix ::= '[' exp ']' | '.' Identifier
         */
        std::shared_ptr<aux::ir::syntax_tree::PrefixExpressionSuffixTree> parsePrefixExprSuffix();

        /**
         * funcCallSuffix ::= [':' Identifier] args
         */
        std::shared_ptr<aux::ir::syntax_tree::FunctionCallSuffixTree> parseFuncCallSuffix();

        /**
         * args ::= '(' [expList] ')' | tableConstructor | LiteralString
         */
        std::shared_ptr<aux::ir::syntax_tree::ArgumentsTree> parseArgs();

        /**
         * tableConstructor ::= '{' [fieldList] '}'
         */
        std::shared_ptr<aux::ir::syntax_tree::TableConstructorTermTree> parseTableConstructor();

        /**
         * fieldList ::= field {(',' | ';') field} [(',' | ';')]
         */
        std::shared_ptr<aux::ir::syntax_tree::TableConstructorTermTree> parseTableFieldList();

        /**
         * field ::= '[' exp ']' '=' exp | Identifier '=' exp | exp
         */
        std::shared_ptr<aux::ir::syntax_tree::TableFieldTermTree> parseTableField();

        /**
         * arlExpression ::= logicalAndTerm {OR logicalOrTerm}
         */
        std::shared_ptr<aux::ir::syntax_tree::TermTree> parseArlExpr();

        /**
         * logicalAndTerm ::= relationalTerm {AND relationalTerm}
         */
        std::shared_ptr<aux::ir::syntax_tree::TermTree> parseLogicalAndTerm();

        /**
         * relationalTerm     ::= bitwiseOrTerm {RelationalOperator bitwiseOrTerm}
         * RelationalOperator ::= ('\<' | '>' | '\<=' | '>=' | '~=' | '==')
         */
        std::shared_ptr<aux::ir::syntax_tree::TermTree> parseRelationalTerm();

        /**
         * bitwiseOrTerm ::= bitwiseXorTerm {'|' bitwiseXorTerm}
         */
        std::shared_ptr<aux::ir::syntax_tree::TermTree> parseBitwiseOrTerm();

        /**
         * bitwiseXorTerm ::= bitwiseAndTerm {'~' bitwiseAndTerm}
         */
        std::shared_ptr<aux::ir::syntax_tree::TermTree> parseBitwiseXorTerm();

        /**
         * bitwiseAndTerm ::= shiftedTerm {'&' shiftedTerm}
         */
        std::shared_ptr<aux::ir::syntax_tree::TermTree> parseBitwiseAndTerm();

        /**
         * shiftedTerm ::= stringConcatenationTerm {('<<' | '>>') stringConcatenationTerm
         */
        std::shared_ptr<aux::ir::syntax_tree::TermTree> parseShiftedTerm();

        /**
         * stringConcatenationTerm ::= summationTerm {'..' summationTerm}
         */
        std::shared_ptr<aux::ir::syntax_tree::TermTree> parseStringConcatenationTerm();

        /**
         * summationTerm :== productTerm {('+' | '-') productTerm}
         */
        std::shared_ptr<aux::ir::syntax_tree::TermTree> parseSummationTerm();

        /**
         * productTerm ::= unaryTerm {('*' | '/' | '//' | '%')  productTerm}
         */
        std::shared_ptr<ir::syntax_tree::TermTree> parseProductTerm();

        /**
         * unaryTerm ::= [(NOT | '#' | '-' | '~')] exponentTerm
         */
        std::shared_ptr<ir::syntax_tree::TermTree> parseUnaryTerm();

        /**
         * exponentTerm ::= term ['^' exponentTerm]
         */
        std::shared_ptr<ir::syntax_tree::TermTree> parseExponentTerm();

        /**
         * term ::= NIL | FALSE | TRUE | Numeral | LiteralString | prefixExpr
         */
        std::shared_ptr<ir::syntax_tree::TermTree> parseTerm();

    };

}


#endif //AUX_PARSER_H
