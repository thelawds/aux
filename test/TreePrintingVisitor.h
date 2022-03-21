//
// Created by miserable on 09.03.2022.
//

#ifndef AUX_TREEPRINTINGVISITOR_H
#define AUX_TREEPRINTINGVISITOR_H

#include "../src/semantics/AstVisitor.h"
#include "../src/intermediate_representation/tree/SyntaxTree.h"
#include <string>
#include <stack>

#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>

class TreePrintingVisitor : public aux::ir::semantics::AstVisitor {
public:

    void printTree(const std::string &path, aux::ir::syntax_tree::AbstractSyntaxTree *tree);

    void visitKeywordTerm(aux::ir::syntax_tree::KeywordTermTree *tree) override;

    void visitKeywordStatementTree(aux::ir::syntax_tree::KeywordStatementTree *tree) override;

    void visitGotoStatementTree(aux::ir::syntax_tree::GotoStatementTree *tree) override;

    void visitIdentifierTerm(aux::ir::syntax_tree::IdentifierTermTree *tree) override;

    void visitIntegerTerm(aux::ir::syntax_tree::IntegerTermTree *tree) override;

    void visitDoubleTerm(aux::ir::syntax_tree::DoubleTermTree *tree) override;

    void visitStringLiteralTerm(aux::ir::syntax_tree::StringLiteralTermTree *tree) override;

    void visitPrefixExpressionTermTree(aux::ir::syntax_tree::PrefixExpressionTermTree *tree) override;

    void visitExponentTermTree(aux::ir::syntax_tree::ExponentTermTree *tree) override;

    void visitUnaryTermTree(aux::ir::syntax_tree::UnaryTermTree *tree) override;

    void visitBinaryExpressionTermTree(aux::ir::syntax_tree::BinaryExpressionTermTree *tree) override;

    void visitLogicalExpressionTermTree(aux::ir::syntax_tree::LogicalExpressionTermTree *tree) override;

    void visitTableFieldTermTree(aux::ir::syntax_tree::TableFieldTermTree *tree) override;

    void visitTableConstructorTermTree(aux::ir::syntax_tree::TableConstructorTermTree *tree) override;

    void visitExpressionListTree(aux::ir::syntax_tree::ExpressionListTree *tree) override;

    void visitFunctionCallSuffixTree(aux::ir::syntax_tree::FunctionCallSuffixTree *tree) override;

    void visitTableFieldAccessSuffixTree(aux::ir::syntax_tree::TableFieldAccessSuffixTree *tree) override;

    void visitStructAccessSuffixTree(aux::ir::syntax_tree::StructAccessSuffixTree *tree) override;

    void visitVariableTree(aux::ir::syntax_tree::VariableTree *tree) override;

    void visitVariableListTree(aux::ir::syntax_tree::VariableListTree *tree) override;

    void visitAttributeTree(aux::ir::syntax_tree::AttributeTree *tree) override;

    void visitIdentifierTermListTree(aux::ir::syntax_tree::IdentifierTermListTree *tree) override;

    void visitParameterListTree(aux::ir::syntax_tree::ParameterListTree *tree) override;

    void visitAttributeIdentifierListTree(aux::ir::syntax_tree::AttributeIdentifierListTree *tree) override;

    void visitLabelTree(aux::ir::syntax_tree::LabelTree *tree) override;

    void visitFunctionBodyTree(aux::ir::syntax_tree::FunctionBodyTree *tree) override;

    void visitFunctionIdentifierTree(aux::ir::syntax_tree::FunctionIdentifierTree *tree) override;

    void visitAssignmentTree(aux::ir::syntax_tree::AssignmentTree *tree) override;

    void visitAttributeIdentifierAssignmentTree(aux::ir::syntax_tree::AttributeIdentifierAssignmentTree *tree) override;

    void visitReturnStatementTree(aux::ir::syntax_tree::ReturnStatementTree *tree) override;

    void visitForLoopStatementTree(aux::ir::syntax_tree::ForLoopStatementTree *tree) override;

    void visitFunctionDefinitionTree(aux::ir::syntax_tree::FunctionDefinitionTree *tree) override;

    void visitWhileLoopTree(aux::ir::syntax_tree::WhileLoopTree *tree) override;

    void visitRepeatUntilTree(aux::ir::syntax_tree::RepeatUntilTree *tree) override;

    void visitIfThenElseStatementTree(aux::ir::syntax_tree::IfThenElseStatementTree *tree) override;

    void visitProgramTree(aux::ir::syntax_tree::ProgramTree *tree) override;

private:
    ogdf::Graph graph;
    ogdf::GraphAttributes graphAttributes;
    ogdf::SugiyamaLayout sugiyamaLayout;
    std::stack<ogdf::node> nodes;

    ogdf::node &newNode(const std::string_view &label);

    void finishCurrentNodeAsRightSubtree(const std::string &label = "");

    void finishCurrentNodeAsLeftSubtree(const std::string &label = "");
};


#endif //AUX_TREEPRINTINGVISITOR_H
