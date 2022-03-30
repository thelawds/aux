/**
 * @author miserable_fx             \n
 * @Handles                         \n
 * - Function Calls                 \n
 * - Table Constructors             \n
 * - Table Access Operators         \n
 * - Function Definitions           \n
*/

#include "semantics/transformation/AstToProgramTreeTransformationVisitor.h"
#include "intermediate_representation/tree/SyntaxTree.h"

using namespace aux::semantics;
using namespace aux::ir::syntax_tree;


/**
 * ----------------------------------------------------            \n
 * \b Handles <i> Table Constructors and Function Definitions </i> \n
 * ----------------------------------------------------
 */

void AstToProgramTreeTransformationVisitor::visitFunctionDefinitionTree(FunctionDefinitionTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitFunctionIdentifierTree(FunctionIdentifierTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitFunctionBodyTree(FunctionBodyTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitParameterListTree(ParameterListTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitTableConstructorTermTree(TableConstructorTermTree *tree) {
    // Returns only empty table constructor. Fields are implemented during visiting assignment
    stackPush(
            std::make_shared<aux::ir::program_tree::expression::TableConstructorTerm>()
    );
}

void AstToProgramTreeTransformationVisitor::visitTableFieldTermTree(TableFieldTermTree *tree) {
    // Unused. Fields are implemented during visiting assignment
}

/**
 * --------------------------------------------------            \n
 * \b Handles <i> Function Calls and Table Access Operators </i> \n
 * --------------------------------------------------
 */

void AstToProgramTreeTransformationVisitor::visitPrefixExpressionTermTree(PrefixExpressionTermTree *tree) {
    auto result = std::dynamic_pointer_cast<aux::ir::program_tree::ExpressionTree>(
            visit(tree->expressionOrIdentifier)
    );

    for (auto &suf: tree->peSuffixTrees) {

        if (auto funcCallSuffix = std::dynamic_pointer_cast<FunctionCallSuffixTree>(suf)) {
            // todo: now function:identifier(args) is ignored and read as function(args)

            auto &arguments = funcCallSuffix->arguments;
            auto newResult = std::make_shared<aux::ir::program_tree::expression::FunctionCallTerm>(result);

            bool isSingleArgument = std::dynamic_pointer_cast<StringLiteralTermTree>(arguments)
                                    || std::dynamic_pointer_cast<TableConstructorTermTree>(arguments);

            if (isSingleArgument) {
                newResult->append(
                        std::dynamic_pointer_cast<aux::ir::program_tree::ExpressionTree>(
                                visit(arguments)
                        )
                );
            } else if (auto argumentsList = std::dynamic_pointer_cast<ExpressionListTree>(arguments)) {
                for (auto &arg: argumentsList->expressions) {
                    newResult->append(
                            std::dynamic_pointer_cast<aux::ir::program_tree::ExpressionTree>(
                                    visit(arg)
                            )
                    );
                }
            }

            result = newResult;
        } else {
            auto programTreeSuffix = std::dynamic_pointer_cast<aux::ir::program_tree::ExpressionTree>(visit(suf));
            result = std::make_shared<aux::ir::program_tree::expression::TableAccessTerm>(
                    result, programTreeSuffix
            );
        }
    }

    stackPush(result);
}

void AstToProgramTreeTransformationVisitor::visitFunctionCallSuffixTree(FunctionCallSuffixTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitTableFieldAccessSuffixTree(TableFieldAccessSuffixTree *tree) {
    stackPush(visit(tree->expressionTree));
}

void AstToProgramTreeTransformationVisitor::visitStructAccessSuffixTree(StructAccessSuffixTree *tree) {
    stackPush(
            std::make_shared<aux::ir::program_tree::expression::ExpressionTerm>(
                    tree->identifier->getValue(),
                    aux::ir::program_tree::expression::ExpressionTerm::TermType::STRING_LITERAL
            )
    );
}
