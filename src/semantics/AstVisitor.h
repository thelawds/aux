//
// Created by miserable on 03.03.2022.
//

#ifndef AUX_ASTVISITOR_H
#define AUX_ASTVISITOR_H


namespace aux::ir::tree {
    struct AbstractSyntaxTree;
    struct TermTree;
    struct KeywordTermTree;
    struct KeywordStatementTree;
    struct GotoStatementTree;
    struct IdentifierTermTree;
    struct IntegerTermTree;
    struct DoubleTermTree;
    struct StringLiteralTermTree;
    struct PrefixExpressionTermTree;
    struct ExponentTermTree;
    struct UnaryTermTree;
    struct BinaryExpressionTermTree;
    struct LogicalExpressionTermTree;
    struct TableFieldTermTree;
    struct TableConstructorTermTree;
    struct ExpressionListTree;
    struct FunctionCallSuffixTree;
    struct TableFieldAccessSuffixTree;
    struct StructAccessSuffixTree;
    struct VariableTree;
    struct VariableListTree;
    struct AttributeTree;
    struct IdentifierTermListTree;
    struct ParameterListTree;
    struct AttributeIdentifierListTree;
    struct LabelTree;
    struct FunctionBodyTree;
    struct FunctionIdentifierTree;
    struct AssignmentTree;
    struct AttributeIdentifierAssignmentTree;
    struct ReturnStatementTree;
    struct ForLoopStatementTree;
    struct FunctionDefinitionTree;
    struct WhileLoopTree;
    struct RepeatUntilTree;
    struct IfThenElseStatementTree;
    struct ProgramTree;
}

namespace aux::ir::semantics {

    struct AstVisitor {

        virtual void visitKeywordTerm(aux::ir::tree::KeywordTermTree *) = 0;

        virtual void visitKeywordStatementTree(aux::ir::tree::KeywordStatementTree *) = 0;

        virtual void visitGotoStatementTree(aux::ir::tree::GotoStatementTree *) = 0;

        virtual void visitIdentifierTerm(aux::ir::tree::IdentifierTermTree *) = 0;

        virtual void visitIntegerTerm(aux::ir::tree::IntegerTermTree *) = 0;

        virtual void visitDoubleTerm(aux::ir::tree::DoubleTermTree *) = 0;

        virtual void visitStringLiteralTerm(aux::ir::tree::StringLiteralTermTree *) = 0;

        virtual void visitPrefixExpressionTermTree(aux::ir::tree::PrefixExpressionTermTree *) = 0;

        virtual void visitExponentTermTree(aux::ir::tree::ExponentTermTree *) = 0;

        virtual void visitUnaryTermTree(aux::ir::tree::UnaryTermTree *) = 0;

        virtual void visitBinaryExpressionTermTree(aux::ir::tree::BinaryExpressionTermTree *) = 0;

        virtual void visitLogicalExpressionTermTree(tree::LogicalExpressionTermTree *) = 0;

        virtual void visitTableFieldTermTree(aux::ir::tree::TableFieldTermTree *) = 0;

        virtual void visitTableConstructorTermTree(aux::ir::tree::TableConstructorTermTree *) = 0;

        virtual void visitExpressionListTree(aux::ir::tree::ExpressionListTree *) = 0;

        virtual void visitFunctionCallSuffixTree(aux::ir::tree::FunctionCallSuffixTree *) = 0;

        virtual void visitTableFieldAccessSuffixTree(aux::ir::tree::TableFieldAccessSuffixTree *) = 0;

        virtual void visitStructAccessSuffixTree(aux::ir::tree::StructAccessSuffixTree *) = 0;

        virtual void visitVariableTree(aux::ir::tree::VariableTree *) = 0;

        virtual void visitVariableListTree(aux::ir::tree::VariableListTree *) = 0;

        virtual void visitAttributeTree(aux::ir::tree::AttributeTree *) = 0;

        virtual void visitIdentifierTermListTree(aux::ir::tree::IdentifierTermListTree *) = 0;

        virtual void visitParameterListTree(aux::ir::tree::ParameterListTree *) = 0;

        virtual void visitAttributeIdentifierListTree(aux::ir::tree::AttributeIdentifierListTree *) = 0;

        virtual void visitLabelTree(aux::ir::tree::LabelTree *) = 0;

        virtual void visitFunctionBodyTree(aux::ir::tree::FunctionBodyTree *) = 0;

        virtual void visitFunctionIdentifierTree(aux::ir::tree::FunctionIdentifierTree *) = 0;

        virtual void visitAssignmentTree(aux::ir::tree::AssignmentTree *) = 0;

        virtual void visitAttributeIdentifierAssignmentTree(aux::ir::tree::AttributeIdentifierAssignmentTree *) = 0;

        virtual void visitReturnStatementTree(aux::ir::tree::ReturnStatementTree *) = 0;

        virtual void visitForLoopStatementTree(aux::ir::tree::ForLoopStatementTree *) = 0;

        virtual void visitFunctionDefinitionTree(aux::ir::tree::FunctionDefinitionTree *) = 0;

        virtual void visitWhileLoopTree(aux::ir::tree::WhileLoopTree *) = 0;

        virtual void visitRepeatUntilTree(aux::ir::tree::RepeatUntilTree *) = 0;

        virtual void visitIfThenElseStatementTree(aux::ir::tree::IfThenElseStatementTree *) = 0;

        virtual void visitProgramTree(aux::ir::tree::ProgramTree *) = 0;
    };

}


#endif //AUX_ASTVISITOR_H
