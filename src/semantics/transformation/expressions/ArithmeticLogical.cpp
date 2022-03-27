/**
 * @author miserable_fx             \n
 * @Handles                         \n
 * - Arithmetic Expressions         \n
 * - Relational Expressions         \n
 * - Logical Expressions            \n
*/

#include "semantics/transformation/AstToProgramTreeTransformationVisitor.h"
#include "intermediate_representation/tree/SyntaxTree.h"

using namespace aux::ir;
using namespace aux::semantics;
using namespace aux::ir::syntax_tree;

/**
 * -------------------------------------------              \n
 * \b Handles <i> Literals, Variable References, NIL </i>   \n
 * -------------------------------------------
 */

void AstToProgramTreeTransformationVisitor::visitKeywordTerm(KeywordTermTree *tree) {
    if (tree->keyword == ir::tokens::Keyword::NIL) {
        stackPush(std::make_shared<program_tree::expression::NilTerm>());
    } else if (tree->keyword == ir::tokens::Keyword::TRUE) {
        stackPush(
                std::make_shared<program_tree::expression::ExpressionTerm>(
                        "1", program_tree::expression::ExpressionTerm::BOOLEAN
                )
        );
    } else if (tree->keyword == ir::tokens::Keyword::FALSE) {
        stackPush(
                std::make_shared<program_tree::expression::ExpressionTerm>(
                        "0", program_tree::expression::ExpressionTerm::BOOLEAN
                )
        );
    } else {
        throw std::invalid_argument("Unexpected keyword " + *tree->keyword); // todo: change error
    }
}

void AstToProgramTreeTransformationVisitor::visitIdentifierTerm(IdentifierTermTree *tree) {
    stackPush(
            std::make_shared<program_tree::expression::ExpressionTerm>(
                    tree->value, program_tree::expression::ExpressionTerm::IDENTIFIER
            )
    );
}

void AstToProgramTreeTransformationVisitor::visitIntegerTerm(IntegerTermTree *tree) {
    stackPush(
            std::make_shared<program_tree::expression::ExpressionTerm>(
                    std::to_string(tree->value), program_tree::expression::ExpressionTerm::INTEGER
            )
    );
}

void AstToProgramTreeTransformationVisitor::visitDoubleTerm(DoubleTermTree *tree) {
    stackPush(
            std::make_shared<program_tree::expression::ExpressionTerm>(
                    std::to_string(tree->value), program_tree::expression::ExpressionTerm::FLOAT
            )
    );
}

void AstToProgramTreeTransformationVisitor::visitStringLiteralTerm(StringLiteralTermTree *tree) {
    stackPush(
            std::make_shared<program_tree::expression::ExpressionTerm>(
                    tree->value, program_tree::expression::ExpressionTerm::STRING_LITERAL
            )
    );
}

/**
 * ---------------------------------------            \n
 * \b Handles <i> Arithmetic-Logical Expressions </i> \n
 * ---------------------------------------
 */

void AstToProgramTreeTransformationVisitor::visitExponentTermTree(ExponentTermTree *tree) {
    auto left = visit(tree->left);
    auto right = visit(tree->right);

    std::shared_ptr<program_tree::ExpressionTree> leftExpr, rightExpr;

    if (
            (leftExpr = std::dynamic_pointer_cast<program_tree::ExpressionTree>(left))
            && (rightExpr = std::dynamic_pointer_cast<program_tree::ExpressionTree>(right))
    ) {
        stackPush(
                std::make_shared<program_tree::expression::ArithmeticExpression>(
                        leftExpr, rightExpr, program_tree::expression::ArithmeticExpression::EXPONENTIATION
                )
        );
    } else {
        throw std::logic_error("Left or right of Exponent Term is not an Expression"); // todo: change error
    }

}

void AstToProgramTreeTransformationVisitor::visitUnaryTermTree(UnaryTermTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitBinaryExpressionTermTree(BinaryExpressionTermTree *tree) {
    auto left = visit(tree->left);
    auto right = visit(tree->right);

    std::shared_ptr<program_tree::ExpressionTree> leftExpr, rightExpr;

    if (
            (leftExpr = std::dynamic_pointer_cast<program_tree::ExpressionTree>(left))
            && (rightExpr = std::dynamic_pointer_cast<program_tree::ExpressionTree>(right))
            ) {
        stackPush(
                std::make_shared<program_tree::expression::ArithmeticExpression>(
                        leftExpr, rightExpr, tree->binaryOperator // todo: String concatenation, relational and binary operations
                )
        );
    } else {
        throw std::logic_error("Left or right of Exponent Term is not an Expression"); // todo: change error
    }
}

void AstToProgramTreeTransformationVisitor::visitLogicalExpressionTermTree(LogicalExpressionTermTree *tree) {

}
