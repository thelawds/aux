//
// Created by miserable on 10.02.2022.
//

#ifndef AUX_PARSER_H
#define AUX_PARSER_H

#include "../scanner/IScanner.h"
#include "../intermediate_representation/ast/ArithmeticLogicalExpressionTree.h"
#include "../intermediate_representation/ast/ArgsTree.h"
#include "../intermediate_representation/ast/VarTree.h"
#include "../intermediate_representation/ast/FunctionCallTree.h"
#include "../intermediate_representation/ast/PeSuffixTree.h"
#include "../intermediate_representation/ast/PrefixExprTree.h"

namespace aux::parser {

    struct Parser {

        explicit Parser(std::shared_ptr<scanner::IScanner> scanner);

        std::shared_ptr<ir::ast::TreeNode> parse();

    private:
        std::shared_ptr<scanner::IScanner> _scanner;

        /**
         * Todo: the grammar here is incorrect for testing purposes. Replace it
         * @return
         */
        std::shared_ptr<ir::ast::TreeNode> parseExp();

        /**
         * functionCall ::= peSuffix funcCallSuffix
         * @return the corresponding tree
         */
        std::shared_ptr<ir::ast::TreeNode> parseFunctionCall();

        /**
         * peSuffix ::= (Identifier | '(' exp ')') {peSuffix | funcCallSuffix}
         * @return the corresponding tree
         */
        std::shared_ptr<ir::ast::PrefixExprTree> parsePrefixExp();

        /**
         * var ::= (Identifier | '(' exp ')')  {peSuffix}
         * @return the corresponding tree
         */
        std::shared_ptr<ir::ast::VarTree> parseVar();

        /**
         * peSuffix ::= '[' exp ']' | '.' Identifier
         * @return
         */
        std::shared_ptr<ir::ast::PeSuffixTree> parsePeSuffix();

        /**
         * funcCallSuffix ::= [':' Identifier] args
         * @return
         */
        std::shared_ptr<ir::ast::FunctionCallTree> parseFuncCallSuffix();

        /**
         * TODO: The grammar is incorrect for testing purposes
         * args ::= '(' ')'
         * @return
         */
        std::shared_ptr<ir::ast::ArgsTree> parseArgs();

    };

}


#endif //AUX_PARSER_H
