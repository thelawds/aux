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
 * ----------------------------------              \n
 * \b Handles <i> Assignment, Function Call </i>   \n
 * ----------------------------------
 */

void AstToProgramTreeTransformationVisitor::visitAttributeIdentifierListTree(AttributeIdentifierListTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitVariableTree(VariableTree *tree) {
    using namespace aux::ir::program_tree::statement;
    using aux::ir::program_tree::ExpressionTree;

    if (tree->prefixExpressionSuffix.empty()) {
        stackPush(
                std::make_shared<IdentifierReferenceTree>(
                        tree->identifier->value
                )
        );
    } else {
        auto tableReferenceTree = std::make_shared<TableReferenceTree>(
                std::make_shared<IdentifierReferenceTree>(tree->identifier->value),
                std::dynamic_pointer_cast<ExpressionTree>(visit(tree->prefixExpressionSuffix[0]))
        );

        for (int i = 1; i < tree->prefixExpressionSuffix.size(); ++i) {
            tableReferenceTree = std::make_shared<TableReferenceTree>(
                    tableReferenceTree,
                    std::dynamic_pointer_cast<ExpressionTree>(visit(tree->prefixExpressionSuffix[i]))
            );
        }

        stackPush(tableReferenceTree);
    }
}

void AstToProgramTreeTransformationVisitor::visitVariableListTree(VariableListTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitAttributeTree(AttributeTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitAssignmentTree(AssignmentTree *tree) {
    std::vector<std::shared_ptr<aux::ir::program_tree::statement::VariableReferenceTree>> programTreeVariables;
    std::vector<std::shared_ptr<aux::ir::program_tree::ExpressionTree>> programTreeExpressions;

    auto astVariables = tree->variableList->variableTrees;
    auto astExpressions = tree->expressionList->expressions;

    for (auto &variable: astVariables) {
        programTreeVariables.push_back(
                std::dynamic_pointer_cast<aux::ir::program_tree::statement::VariableReferenceTree>(
                        visit(variable)
                )
        );
    }

    for (auto &expression: astExpressions) {
        programTreeExpressions.push_back(
                std::dynamic_pointer_cast<aux::ir::program_tree::ExpressionTree>(
                        visit(expression)
                )
        );
    }

    size_t idx = 0;
    size_t minSize = std::min(programTreeVariables.size(), programTreeExpressions.size());
    auto resultAssignmentTree = std::make_shared<ir::program_tree::statement::AssignmentStatement>();
    while (idx < minSize) {
        resultAssignmentTree->append(programTreeVariables[idx], programTreeExpressions[idx]);
        ++idx;
    }

    while (idx < programTreeVariables.size()) {
        resultAssignmentTree->append(programTreeVariables[idx]);
        ++idx;
    }

    stackPush(resultAssignmentTree);
}

void AstToProgramTreeTransformationVisitor::visitAttributeIdentifierAssignmentTree(
        AttributeIdentifierAssignmentTree *tree
) {

}
