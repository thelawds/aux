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

    void printTree(const std::string &path, aux::ir::tree::AbstractSyntaxTree *tree);

    void visitKeywordTerm(aux::ir::tree::KeywordTermTree *tree) override;

    void visitKeywordStatementTree(aux::ir::tree::KeywordStatementTree *tree) override;

    void visitGotoStatementTree(aux::ir::tree::GotoStatementTree *tree) override;

    void visitIdentifierTerm(aux::ir::tree::IdentifierTermTree *tree) override;

    void visitIntegerTerm(aux::ir::tree::IntegerTermTree *tree) override;

    void visitDoubleTerm(aux::ir::tree::DoubleTermTree *tree) override;

    void visitStringLiteralTerm(aux::ir::tree::StringLiteralTermTree *tree) override;

    void visitPrefixExpressionTermTree(aux::ir::tree::PrefixExpressionTermTree *tree) override;

    void visitExponentTermTree(aux::ir::tree::ExponentTermTree *tree) override;

    void visitUnaryTermTree(aux::ir::tree::UnaryTermTree *tree) override;

    void visitBinaryExpressionTermTree(aux::ir::tree::BinaryExpressionTermTree *tree) override;

    void visitLogicalExpressionTermTree(aux::ir::tree::LogicalExpressionTermTree *tree) override;

    void visitTableFieldTermTree(aux::ir::tree::TableFieldTermTree *tree) override;

    void visitTableConstructorTermTree(aux::ir::tree::TableConstructorTermTree *tree) override;

    void visitExpressionListTree(aux::ir::tree::ExpressionListTree *tree) override;

    void visitFunctionCallSuffixTree(aux::ir::tree::FunctionCallSuffixTree *tree) override;

    void visitTableFieldAccessSuffixTree(aux::ir::tree::TableFieldAccessSuffixTree *tree) override;

    void visitStructAccessSuffixTree(aux::ir::tree::StructAccessSuffixTree *tree) override;

    void visitVariableTree(aux::ir::tree::VariableTree *tree) override;

    void visitVariableListTree(aux::ir::tree::VariableListTree *tree) override;

    void visitAttributeTree(aux::ir::tree::AttributeTree *tree) override;

    void visitIdentifierTermListTree(aux::ir::tree::IdentifierTermListTree *tree) override;

    void visitParameterListTree(aux::ir::tree::ParameterListTree *tree) override;

    void visitAttributeIdentifierListTree(aux::ir::tree::AttributeIdentifierListTree *tree) override;

    void visitLabelTree(aux::ir::tree::LabelTree *tree) override;

    void visitFunctionBodyTree(aux::ir::tree::FunctionBodyTree *tree) override;

    void visitFunctionIdentifierTree(aux::ir::tree::FunctionIdentifierTree *tree) override;

    void visitAssignmentTree(aux::ir::tree::AssignmentTree *tree) override;

    void visitAttributeIdentifierAssignmentTree(aux::ir::tree::AttributeIdentifierAssignmentTree *tree) override;

    void visitReturnStatementTree(aux::ir::tree::ReturnStatementTree *tree) override;

    void visitForLoopStatementTree(aux::ir::tree::ForLoopStatementTree *tree) override;

    void visitFunctionDefinitionTree(aux::ir::tree::FunctionDefinitionTree *tree) override;

    void visitWhileLoopTree(aux::ir::tree::WhileLoopTree *tree) override;

    void visitRepeatUntilTree(aux::ir::tree::RepeatUntilTree *tree) override;

    void visitIfThenElseStatementTree(aux::ir::tree::IfThenElseStatementTree *tree) override;

    void visitProgramTree(aux::ir::tree::ProgramTree *tree) override;

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
