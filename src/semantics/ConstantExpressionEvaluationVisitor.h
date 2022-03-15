//
// Created by miserable on 12.03.2022.
//

#ifndef AUX_CONSTANTEXPRESSIONEVALUATIONVISITOR_H
#define AUX_CONSTANTEXPRESSIONEVALUATIONVISITOR_H

#include "Visitor.h"
#include <string>
#include "../intermediate_representation/Token.h"

namespace aux::ir::semantics {

    struct ConstantExpressionEvaluationVisitor : Visitor {
        void visitKeywordTerm(aux::ir::tree::KeywordTermTree *termTree) override;

        void visitKeywordStatementTree(aux::ir::tree::KeywordStatementTree *statementTree) override;

        void visitGotoStatementTree(aux::ir::tree::GotoStatementTree *statementTree) override;

        void visitIdentifierTerm(aux::ir::tree::IdentifierTermTree *termTree) override;

        void visitIntegerTerm(aux::ir::tree::IntegerTermTree *termTree) override;

        void visitDoubleTerm(aux::ir::tree::DoubleTermTree *termTree) override;

        void visitStringLiteralTerm(aux::ir::tree::StringLiteralTermTree *termTree) override;

        void visitPrefixExpressionTermTree(aux::ir::tree::PrefixExpressionTermTree *termTree) override;

        void visitExponentTermTree(aux::ir::tree::ExponentTermTree *termTree) override;

        void visitUnaryTermTree(aux::ir::tree::UnaryTermTree *termTree) override;

        void visitBinaryExpressionTermTree(aux::ir::tree::BinaryExpressionTermTree *termTree) override;

        void visitLogicalExpressionTermTree(tree::LogicalExpressionTermTree *termTree) override;

        void visitTableFieldTermTree(aux::ir::tree::TableFieldTermTree *termTree) override;

        void visitTableConstructorTermTree(aux::ir::tree::TableConstructorTermTree *termTree) override;

        void visitExpressionListTree(aux::ir::tree::ExpressionListTree *listTree) override;

        void visitFunctionCallSuffixTree(aux::ir::tree::FunctionCallSuffixTree *suffixTree) override;

        void visitTableFieldAccessSuffixTree(aux::ir::tree::TableFieldAccessSuffixTree *suffixTree) override;

        void visitStructAccessSuffixTree(aux::ir::tree::StructAccessSuffixTree *suffixTree) override;

        void visitVariableTree(aux::ir::tree::VariableTree *variableTree) override;

        void visitVariableListTree(aux::ir::tree::VariableListTree *listTree) override;

        void visitAttributeTree(aux::ir::tree::AttributeTree *attributeTree) override;

        void visitIdentifierTermListTree(aux::ir::tree::IdentifierTermListTree *listTree) override;

        void visitParameterListTree(aux::ir::tree::ParameterListTree *listTree) override;

        void visitAttributeIdentifierListTree(aux::ir::tree::AttributeIdentifierListTree *listTree) override;

        void visitLabelTree(aux::ir::tree::LabelTree *labelTree) override;

        void visitFunctionBodyTree(aux::ir::tree::FunctionBodyTree *bodyTree) override;

        void visitFunctionIdentifierTree(aux::ir::tree::FunctionIdentifierTree *identifierTree) override;

        void visitAssignmentTree(aux::ir::tree::AssignmentTree *assignmentTree) override;

        void
        visitAttributeIdentifierAssignmentTree(aux::ir::tree::AttributeIdentifierAssignmentTree *assignmentTree) override;

        void visitReturnStatementTree(aux::ir::tree::ReturnStatementTree *statementTree) override;

        void visitForLoopStatementTree(aux::ir::tree::ForLoopStatementTree *statementTree) override;

        void visitFunctionDefinitionTree(aux::ir::tree::FunctionDefinitionTree *definitionTree) override;

        void visitWhileLoopTree(aux::ir::tree::WhileLoopTree *loopTree) override;

        void visitRepeatUntilTree(aux::ir::tree::RepeatUntilTree *untilTree) override;

        void visitIfThenElseStatementTree(aux::ir::tree::IfThenElseStatementTree *statementTree) override;

        void visitProgramTree(aux::ir::tree::ProgramTree *programTree) override;

        void evalAllExpressions(aux::ir::tree::AbstractSyntaxTree *tree);

    private:
        enum class AvailableValue {
            STRING,
            INTEGER,
            DOUBLE,
            BOOLEAN,
            NONE
        };

        std::string stringValue;
        int64_t integerValue;
        bool booleanValue;
        long double doubleValue;

        AvailableValue availableValue{AvailableValue::NONE};

        bool isAvailable();

        std::shared_ptr<aux::ir::tree::TermTree> getAsIs() const;
        long double getAsDouble() const;
        std::string getAsString() const;
        int64_t getAsInteger() const;
        bool getAsBoolean() const;

        long double calculateArithmetic(long double left, long double right, aux::ir::tokens::Operator op) const;
        int64_t calculateBitwise(int64_t left, int64_t right, aux::ir::tokens::Operator op) const;
        bool calculateRelational(
                const std::shared_ptr<aux::ir::tree::TermTree>& left,
                const std::shared_ptr<aux::ir::tree::TermTree>& right,
                aux::ir::tokens::Operator op
        ) const;

    };

}


#endif //AUX_CONSTANTEXPRESSIONEVALUATIONVISITOR_H
