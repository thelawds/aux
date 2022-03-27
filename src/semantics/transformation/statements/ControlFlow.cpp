/**
 * @author miserable_fx             \n
 * @Handles                         \n
 * - Arithmetic Expressions         \n
 * - Relational Expressions         \n
 * - Logical Expressions            \n
*/

#include "semantics/transformation/AstToProgramTreeTransformationVisitor.h"
#include "intermediate_representation/tree/SyntaxTree.h"

using namespace aux::semantics;
using namespace aux::ir::syntax_tree;

/**
 * -------------------------------               \n
 * \b Handles <i> Control-Flow Statements </i>   \n
 * -------------------------------
 */

void AstToProgramTreeTransformationVisitor::visitForLoopStatementTree(ForLoopStatementTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitWhileLoopTree(WhileLoopTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitRepeatUntilTree(RepeatUntilTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitIfThenElseStatementTree(IfThenElseStatementTree *tree) {

}
