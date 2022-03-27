//
// Created by miserable on 03.03.2022.
//

#ifndef AUX_ASTVISITOR_H
#define AUX_ASTVISITOR_H


namespace aux::ir::syntax_tree {
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

namespace aux::semantics {

    struct AstVisitor {

        virtual void visitKeywordTerm(aux::ir::syntax_tree::KeywordTermTree *) = 0;

        virtual void visitKeywordStatementTree(aux::ir::syntax_tree::KeywordStatementTree *) = 0;

        virtual void visitGotoStatementTree(aux::ir::syntax_tree::GotoStatementTree *) = 0;

        virtual void visitIdentifierTerm(aux::ir::syntax_tree::IdentifierTermTree *) = 0;

        virtual void visitIntegerTerm(aux::ir::syntax_tree::IntegerTermTree *) = 0;

        virtual void visitDoubleTerm(aux::ir::syntax_tree::DoubleTermTree *) = 0;

        virtual void visitStringLiteralTerm(aux::ir::syntax_tree::StringLiteralTermTree *) = 0;

        virtual void visitPrefixExpressionTermTree(aux::ir::syntax_tree::PrefixExpressionTermTree *) = 0;

        virtual void visitExponentTermTree(aux::ir::syntax_tree::ExponentTermTree *) = 0;

        virtual void visitUnaryTermTree(aux::ir::syntax_tree::UnaryTermTree *) = 0;

        virtual void visitBinaryExpressionTermTree(aux::ir::syntax_tree::BinaryExpressionTermTree *) = 0;

        virtual void visitLogicalExpressionTermTree(aux::ir::syntax_tree::LogicalExpressionTermTree *) = 0;

        virtual void visitTableFieldTermTree(aux::ir::syntax_tree::TableFieldTermTree *) = 0;

        virtual void visitTableConstructorTermTree(aux::ir::syntax_tree::TableConstructorTermTree *) = 0;

        virtual void visitExpressionListTree(aux::ir::syntax_tree::ExpressionListTree *) = 0;

        virtual void visitFunctionCallSuffixTree(aux::ir::syntax_tree::FunctionCallSuffixTree *) = 0;

        virtual void visitTableFieldAccessSuffixTree(aux::ir::syntax_tree::TableFieldAccessSuffixTree *) = 0;

        virtual void visitStructAccessSuffixTree(aux::ir::syntax_tree::StructAccessSuffixTree *) = 0;

        virtual void visitVariableTree(aux::ir::syntax_tree::VariableTree *) = 0;

        virtual void visitVariableListTree(aux::ir::syntax_tree::VariableListTree *) = 0;

        virtual void visitAttributeTree(aux::ir::syntax_tree::AttributeTree *) = 0;

        virtual void visitIdentifierTermListTree(aux::ir::syntax_tree::IdentifierTermListTree *) = 0;

        virtual void visitParameterListTree(aux::ir::syntax_tree::ParameterListTree *) = 0;

        virtual void visitAttributeIdentifierListTree(aux::ir::syntax_tree::AttributeIdentifierListTree *) = 0;

        virtual void visitLabelTree(aux::ir::syntax_tree::LabelTree *) = 0;

        virtual void visitFunctionBodyTree(aux::ir::syntax_tree::FunctionBodyTree *) = 0;

        virtual void visitFunctionIdentifierTree(aux::ir::syntax_tree::FunctionIdentifierTree *) = 0;

        virtual void visitAssignmentTree(aux::ir::syntax_tree::AssignmentTree *) = 0;

        virtual void visitAttributeIdentifierAssignmentTree(aux::ir::syntax_tree::AttributeIdentifierAssignmentTree *) = 0;

        virtual void visitReturnStatementTree(aux::ir::syntax_tree::ReturnStatementTree *) = 0;

        virtual void visitForLoopStatementTree(aux::ir::syntax_tree::ForLoopStatementTree *) = 0;

        virtual void visitFunctionDefinitionTree(aux::ir::syntax_tree::FunctionDefinitionTree *) = 0;

        virtual void visitWhileLoopTree(aux::ir::syntax_tree::WhileLoopTree *) = 0;

        virtual void visitRepeatUntilTree(aux::ir::syntax_tree::RepeatUntilTree *) = 0;

        virtual void visitIfThenElseStatementTree(aux::ir::syntax_tree::IfThenElseStatementTree *) = 0;

        virtual void visitProgramTree(aux::ir::syntax_tree::ProgramTree *) = 0;
    };

}


#endif //AUX_ASTVISITOR_H
