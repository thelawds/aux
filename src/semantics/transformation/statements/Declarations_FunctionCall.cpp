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
    auto resultAssignmentTree = std::make_shared<ir::program_tree::statement::AssignmentStatement>();

    auto astVariables = tree->variableList->variableTrees;
    auto astExpressions = tree->expressionList->expressions;

    size_t idx, min_size = std::min(astVariables.size(), astExpressions.size());
    for (idx = 0; idx < min_size; ++idx) {
        auto &variableReference = astVariables[idx];
        auto &expression = astExpressions[idx];

        resultAssignmentTree->append(
                std::dynamic_pointer_cast<aux::ir::program_tree::statement::VariableReferenceTree>(
                        visit(variableReference)
                ),
                std::dynamic_pointer_cast<aux::ir::program_tree::ExpressionTree>(
                        visit(expression)
                )
        );

        if (auto tableConstructorExpression = std::dynamic_pointer_cast<TableConstructorTermTree>(expression)) {
            auto &fields = tableConstructorExpression->fields;
            int fieldIndex = 1;

            for (auto &field: fields) {
                auto variableTreeForCurrentField = std::make_shared<VariableTree>(
                        variableReference->identifier, variableReference->prefixExpressionSuffix
                );
                if (field->left) {
                    if (auto identifierFieldLeft = std::dynamic_pointer_cast<IdentifierTermTree>(field->left)) {
                        variableTreeForCurrentField->addPrefixExprSuffix(
                                std::make_shared<StructAccessSuffixTree>(
                                        std::make_shared<TokenIdentifier>(
                                                identifierFieldLeft->value, identifierFieldLeft->span
                                        )
                                )
                        );
                    } else {
                        variableTreeForCurrentField->addPrefixExprSuffix(
                                std::make_shared<TableFieldAccessSuffixTree>(field->left)
                        );
                    }
                } else {
                    variableTreeForCurrentField->addPrefixExprSuffix(
                            std::make_shared<IntegerTermTree>(fieldIndex)
                    );
                    ++fieldIndex;
                }

                resultAssignmentTree->append(
                        std::dynamic_pointer_cast<aux::ir::program_tree::statement::VariableReferenceTree>(
                                visit(variableTreeForCurrentField)
                        ),
                        std::dynamic_pointer_cast<aux::ir::program_tree::ExpressionTree>(
                                visit(field->right)
                        )
                );
            }
        }
    }

    while (idx < astVariables.size()) {
        resultAssignmentTree->append(
                std::dynamic_pointer_cast<aux::ir::program_tree::statement::VariableReferenceTree>(
                        visit(astVariables[idx])
                )
        );
        ++idx;
    }

    stackPush(resultAssignmentTree);
}

void AstToProgramTreeTransformationVisitor::visitAttributeIdentifierAssignmentTree(
        AttributeIdentifierAssignmentTree *tree
) {

}
