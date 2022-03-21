#include "ProgramTree.h"

#include <utility>
#include "semantics/ProgramTreeVisitor.h"

using namespace std;
using namespace aux::semantics;
using namespace aux::ir::tokens;
using namespace aux::ir::program_tree;
using namespace aux::ir::program_tree::expression;
using namespace aux::ir::program_tree::statement;

void NilTerm::accept(ProgramTreeVisitor *visitor) {
    visitor->visitNilTerm(this);
}

ExpressionTerm::ExpressionTerm(string value, TermType termType) : value(std::move(value)), termType(termType) {}

void ExpressionTerm::accept(ProgramTreeVisitor *visitor) {
    visitor->visitExpressionTerm(this);
}

ArithmeticExpression::ArithmeticExpression(
        const shared_ptr<ExpressionTree> &left,
        const shared_ptr<ExpressionTree> &right,
        Operator tokenOperator
) : left(left), right(right), operation(convert(tokenOperator)) {}

ArithmeticExpression::ArithmeticExpression(
        const shared_ptr<ExpressionTree> &left,
        const shared_ptr<ExpressionTree> &right,
        ArithmeticExpression::ArithmeticOperation operation
) : left(left), right(right), operation(operation) {}


ArithmeticExpression::ArithmeticOperation ArithmeticExpression::convert(const Operator &tokenOperator) {
    switch (tokenOperator) {
        case Operator::PLUS:
            return ADDITION;
        case Operator::MINUS:
            return SUBTRACTION;
        case Operator::ASTERISK:
            return MULTIPLICATION;
        case Operator::SLASH:
            return DIVISION;
        case Operator::PERCENT:
            return MODULO;
        case Operator::CARET:
            return EXPONENTIATION;
        case Operator::SLASH_SLASH:
            return FLOOR_DIVISION;
        default:
            throw invalid_argument(*tokenOperator + " is not valid arithmetic operator.");
    }
}

void ArithmeticExpression::accept(ProgramTreeVisitor *visitor) {
    visitor->visitArithmeticExpression(this);
}

IdentifierVariableReferenceTree::IdentifierVariableReferenceTree(string identifier)
        : identifier(std::move(identifier)) {}

void IdentifierVariableReferenceTree::accept(aux::semantics::ProgramTreeVisitor *visitor) {
    visitor->visitIdentifierVariableReferenceTree(this);
}

void AssignmentStatement::append(
        const std::shared_ptr<VariableReferenceTree> &variable,
        const std::shared_ptr<ExpressionTree> &expression
) {
    assignments.emplace_back(variable, expression);
}

void AssignmentStatement::append(const shared_ptr<VariableReferenceTree> &variable) {
    assignments.emplace_back(variable, make_shared<NilTerm>());

}

void AssignmentStatement::accept(aux::semantics::ProgramTreeVisitor *visitor) {
    visitor->visitAssignmentStatement(this);
}

void ChunkTree::accept(aux::semantics::ProgramTreeVisitor *visitor) {
    visitor->visitChunkTree(this);
}

void ChunkTree::append(const std::shared_ptr<StatementTree>& statementTree) {
    statements.push_back(statementTree);
}