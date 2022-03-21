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

}

void AstToProgramTreeTransformationVisitor::visitTableFieldTermTree(TableFieldTermTree *tree) {

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

}

void AstToProgramTreeTransformationVisitor::visitStructAccessSuffixTree(StructAccessSuffixTree *tree) {

}





