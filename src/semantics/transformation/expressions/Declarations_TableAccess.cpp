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
    tree->expressionOrIdentifier->accept(this); // todo: now only works for references to variables
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
