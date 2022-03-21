//
// Created by miserable on 19.03.2022.
//

#ifndef AUX_ASTTOPROGRAMTREETRANSFORMATIONVISITOR_H
#define AUX_ASTTOPROGRAMTREETRANSFORMATIONVISITOR_H

#include "semantics/AstVisitor.h"
#include "intermediate_representation/tree/ProgramTree.h"
#include <stack>

namespace aux::semantics {

    using namespace aux::ir::syntax_tree;

    struct AstToProgramTreeTransformationVisitor : AstVisitor {

        std::shared_ptr<aux::ir::program_tree::ProgramTree> transform(aux::ir::syntax_tree::AbstractSyntaxTree *);

        void visitKeywordTerm(KeywordTermTree *tree) override;

        void visitKeywordStatementTree(KeywordStatementTree *tree) override;

        void visitGotoStatementTree(GotoStatementTree *tree) override;

        void visitIdentifierTerm(IdentifierTermTree *tree) override;

        void visitIntegerTerm(IntegerTermTree *tree) override;

        void visitDoubleTerm(DoubleTermTree *tree) override;

        void visitStringLiteralTerm(StringLiteralTermTree *tree) override;

        void visitPrefixExpressionTermTree(PrefixExpressionTermTree *tree) override;

        void visitExponentTermTree(ExponentTermTree *tree) override;

        void visitUnaryTermTree(UnaryTermTree *tree) override;

        void visitBinaryExpressionTermTree(BinaryExpressionTermTree *tree) override;

        void visitLogicalExpressionTermTree(LogicalExpressionTermTree *tree) override;

        void visitTableFieldTermTree(TableFieldTermTree *tree) override;

        void visitTableConstructorTermTree(TableConstructorTermTree *tree) override;

        void visitExpressionListTree(ExpressionListTree *tree) override;

        void visitFunctionCallSuffixTree(FunctionCallSuffixTree *tree) override;

        void visitTableFieldAccessSuffixTree(TableFieldAccessSuffixTree *tree) override;

        void visitStructAccessSuffixTree(StructAccessSuffixTree *tree) override;

        void visitVariableTree(VariableTree *tree) override;

        void visitVariableListTree(VariableListTree *tree) override;

        void visitAttributeTree(AttributeTree *tree) override;

        void visitIdentifierTermListTree(IdentifierTermListTree *tree) override;

        void visitParameterListTree(ParameterListTree *tree) override;

        void visitAttributeIdentifierListTree(AttributeIdentifierListTree *tree) override;

        void visitLabelTree(LabelTree *tree) override;

        void visitFunctionBodyTree(FunctionBodyTree *tree) override;

        void visitFunctionIdentifierTree(FunctionIdentifierTree *tree) override;

        void visitAssignmentTree(AssignmentTree *tree) override;

        void visitAttributeIdentifierAssignmentTree(AttributeIdentifierAssignmentTree *tree) override;

        void visitReturnStatementTree(ReturnStatementTree *tree) override;

        void visitForLoopStatementTree(ForLoopStatementTree *tree) override;

        void visitFunctionDefinitionTree(FunctionDefinitionTree *tree) override;

        void visitWhileLoopTree(WhileLoopTree *tree) override;

        void visitRepeatUntilTree(RepeatUntilTree *tree) override;

        void visitIfThenElseStatementTree(IfThenElseStatementTree *tree) override;

        void visitProgramTree(ProgramTree *tree) override;

    private:
        std::stack<std::shared_ptr<aux::ir::program_tree::ProgramTree>> returnValues;

        void stackPush(const std::shared_ptr<aux::ir::program_tree::ProgramTree> &value);

        std::shared_ptr<aux::ir::program_tree::ProgramTree> stackPop();

        std::shared_ptr<aux::ir::program_tree::ProgramTree> visit(
                const std::shared_ptr<AbstractSyntaxTree>& abstractSyntaxTree
        );

    };
}


#endif //AUX_ASTTOPROGRAMTREETRANSFORMATIONVISITOR_H
