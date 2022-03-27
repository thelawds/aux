//
// Created by miserable on 09.03.2022.
//

#include "TreePrintingVisitor.h"
#include "../src/intermediate_representation/tree/SyntaxTree.h"

using namespace ogdf;
using namespace aux::ir::syntax_tree;

void TreePrintingVisitor::visitKeywordTerm(KeywordTermTree *tree) {
    newNode(*tree->keyword);
}

void TreePrintingVisitor::visitKeywordStatementTree(KeywordStatementTree *tree) {
    newNode(*tree->keyword);
    finishCurrentNodeAsRightSubtree();
}

void TreePrintingVisitor::visitGotoStatementTree(GotoStatementTree *tree) {
    newNode("GOTO");
    newNode(tree->identifier->getValue());
    finishCurrentNodeAsRightSubtree();
}

void TreePrintingVisitor::visitIdentifierTerm(IdentifierTermTree *tree) {
    newNode(tree->value);
}

void TreePrintingVisitor::visitIntegerTerm(IntegerTermTree *tree) {
    newNode(std::to_string(tree->value));
}

void TreePrintingVisitor::visitDoubleTerm(DoubleTermTree *tree) {
    newNode(std::to_string(tree->value));
}

void TreePrintingVisitor::visitStringLiteralTerm(StringLiteralTermTree *tree) {
    newNode("\"" + tree->value + "\"");
}

void TreePrintingVisitor::visitPrefixExpressionTermTree(PrefixExpressionTermTree *tree) {
    newNode("Prefix Expression:");
    tree->expressionOrIdentifier->accept(this);

    int finishEls = 1;
    for (const auto &suf: tree->peSuffixTrees) {
        suf->accept(this);
        ++finishEls;
    }

    while (finishEls-- > 0) {
        finishCurrentNodeAsRightSubtree();
    }
}

void TreePrintingVisitor::visitExponentTermTree(ExponentTermTree *tree) {
    newNode(*Operator::CARET);
    if (tree->left) {
        tree->left->accept(this);
        finishCurrentNodeAsLeftSubtree();
    }

    if (tree->right) {
        tree->right->accept(this);
        finishCurrentNodeAsRightSubtree();
    }

}

void TreePrintingVisitor::visitUnaryTermTree(UnaryTermTree *tree) {
    if (tree->isKeywordOperator) {
        newNode(*tree->unaryTermOperator.keyword);
    } else {
        newNode(*tree->unaryTermOperator.unaryOperator);
    }

    if (tree->exponentTermTree) {
        tree->exponentTermTree->accept(this);
        finishCurrentNodeAsRightSubtree();
    }
}

void TreePrintingVisitor::visitBinaryExpressionTermTree(BinaryExpressionTermTree *tree) {
    newNode(*tree->binaryOperator);

    if (tree->left) {
        tree->left->accept(this);
        finishCurrentNodeAsLeftSubtree();
    }

    if (tree->right) {
        tree->right->accept(this);
        finishCurrentNodeAsRightSubtree();
    }
}

void TreePrintingVisitor::visitLogicalExpressionTermTree(LogicalExpressionTermTree *tree) {
    newNode(*tree->binaryOperator);

    if (tree->left) {
        tree->left->accept(this);
        finishCurrentNodeAsLeftSubtree();
    }

    if (tree->right) {
        tree->right->accept(this);
        finishCurrentNodeAsRightSubtree();
    }
}

void TreePrintingVisitor::visitTableFieldTermTree(TableFieldTermTree *tree) {
    newNode(*tree->binaryOperator);

    if (tree->left) {
        tree->left->accept(this);
        finishCurrentNodeAsLeftSubtree();
    }

    if (tree->right) {
        tree->right->accept(this);
        finishCurrentNodeAsRightSubtree();
    }
}

void TreePrintingVisitor::visitTableConstructorTermTree(TableConstructorTermTree *tree) {
    newNode("Table Constructor: ");
    int finishEls = 0;
    for (const auto &field: tree->fields) {
        field->accept(this);
        ++finishEls;
    }

    while (finishEls-- > 0) {
        finishCurrentNodeAsRightSubtree();
    }
}

void TreePrintingVisitor::visitExpressionListTree(ExpressionListTree *tree) {
    if (tree->expressions.empty()) {
        newNode("Empty Expression List");
        return;
    }

    int finishEls = 0;
    newNode("Expression List:");
    for (const auto &expression: tree->expressions) {
        expression->accept(this);
        ++finishEls;
    }

    if (finishEls-- > 0) {
        finishCurrentNodeAsRightSubtree();
    }
}

void TreePrintingVisitor::visitFunctionCallSuffixTree(FunctionCallSuffixTree *tree) {
    newNode("Function Call:");
    if (tree->identifier) {
        newNode(":" + tree->identifier->getValue());
        finishCurrentNodeAsLeftSubtree("Identifier");
    }

    if (tree->arguments) {
        tree->arguments->accept(this);
        finishCurrentNodeAsRightSubtree("Call Arguments");
    }
}

void TreePrintingVisitor::visitTableFieldAccessSuffixTree(TableFieldAccessSuffixTree *tree) {

    newNode("Table Field [] Access:");
    if (tree->expressionTree) {
        tree->expressionTree->accept(this);
        finishCurrentNodeAsRightSubtree("Expression");
    }

}

void TreePrintingVisitor::visitStructAccessSuffixTree(StructAccessSuffixTree *tree) {
    newNode("." + tree->identifier->getValue());
}

void TreePrintingVisitor::visitVariableTree(VariableTree *tree) {
    newNode("Variable:");
    tree->identifier->accept(this);
    finishCurrentNodeAsLeftSubtree("Expression");

    int finishEls = 0;
    for (const auto &suf: tree->prefixExpressionSuffix) {
        suf->accept(this);
        ++finishEls;
    }

    while (finishEls-- > 0) {
        finishCurrentNodeAsRightSubtree("Prefix-Expression");
    }
}

void TreePrintingVisitor::visitVariableListTree(VariableListTree *tree) {
    if (tree->variableTrees.empty()) {
        newNode("Empty Variable List");
        return;
    }

    int finishEls = 0;
    newNode("Variable List:");
    for (const auto &var: tree->variableTrees) {
        var->accept(this);
        ++finishEls;
    }

    while (finishEls-- > 0) {
        finishCurrentNodeAsRightSubtree();
    }
}

void TreePrintingVisitor::visitAttributeTree(AttributeTree *tree) {
    newNode("<" + tree->value + ">");
}

void TreePrintingVisitor::visitIdentifierTermListTree(IdentifierTermListTree *tree) {
    if (tree->identifiers.empty()) {
        newNode("Empty Identifiers List");
        return;
    }

    int finishEls = 0;
    newNode("Identifier List:");
    for (const auto &identifier: tree->identifiers) {
        identifier->accept(this);
        ++finishEls;
    }

    while (finishEls-- > 0) {
        finishCurrentNodeAsRightSubtree();
    }
}

void TreePrintingVisitor::visitParameterListTree(ParameterListTree *tree) {

    tree->identifierTermListTree->accept(this);
    if (tree->hasTrailing3DotsOperator) {
        newNode("...");
        finishCurrentNodeAsLeftSubtree();
    }

}

void TreePrintingVisitor::visitAttributeIdentifierListTree(AttributeIdentifierListTree *tree) {
    if (tree->attributeIdentifiers.empty()) {
        newNode("Empty Attribute-Identifier List");
        return;
    }

    int finishEls = 0;
    newNode("Attribute-Identifier List:");
    for (const auto &[identifier, attribute]: tree->attributeIdentifiers) {
        newNode("Attribute-Identifier");

        identifier->accept(this);
        finishCurrentNodeAsLeftSubtree("Identifier");

        attribute->accept(this);
        finishCurrentNodeAsRightSubtree("Attribute");

        ++finishEls;

    }

    while (finishEls-- > 0) {
        finishCurrentNodeAsRightSubtree();
    }
}

void TreePrintingVisitor::visitLabelTree(LabelTree *tree) {
    newNode("::" + tree->identifier->getValue() + "::");
}

void TreePrintingVisitor::visitFunctionBodyTree(FunctionBodyTree *tree) {
    newNode("Function Body:");

    tree->parameters->accept(this);
    finishCurrentNodeAsLeftSubtree("Parameters");

    tree->body->accept(this);
    finishCurrentNodeAsRightSubtree("Body");
}

void TreePrintingVisitor::visitFunctionIdentifierTree(FunctionIdentifierTree *tree) {
    newNode(tree->primaryIdentifier->getValue());
    int finishNodes = 0;

    for (const auto &id: tree->secondaryIdentifiers) {
        newNode("." + id->getValue());
        ++finishNodes;
    }

    if (tree->colonIdentifier) {
        newNode(":" + tree->colonIdentifier->getValue());
        ++finishNodes;
    }

    while (finishNodes-- > 0) {
        finishCurrentNodeAsRightSubtree();
    }
}

void TreePrintingVisitor::visitAssignmentTree(AssignmentTree *tree) {
    newNode("=");

    tree->variableList->accept(this);
    finishCurrentNodeAsLeftSubtree();

    tree->expressionList->accept(this);
    finishCurrentNodeAsRightSubtree();
}

void TreePrintingVisitor::visitAttributeIdentifierAssignmentTree(AttributeIdentifierAssignmentTree *tree) {
    newNode("=");

    tree->attributeIdentifierListTree->accept(this);
    finishCurrentNodeAsLeftSubtree();

    tree->expressionListTree->accept(this);
    finishCurrentNodeAsRightSubtree();
}

void TreePrintingVisitor::visitReturnStatementTree(ReturnStatementTree *tree) {
    newNode("RETURN");

    tree->expressions->accept(this);
    finishCurrentNodeAsRightSubtree();
}

void TreePrintingVisitor::visitForLoopStatementTree(ForLoopStatementTree *tree) {
    newNode("For Loop:");

    tree->identifiers->accept(this);
    tree->expressions->accept(this);
    finishCurrentNodeAsRightSubtree("In");
    finishCurrentNodeAsLeftSubtree("Expression");

    tree->body->accept(this);
    finishCurrentNodeAsRightSubtree("Body");
}

void TreePrintingVisitor::visitFunctionDefinitionTree(FunctionDefinitionTree *tree) {
    newNode("Function:");
    tree->identifier->accept(this);
    finishCurrentNodeAsLeftSubtree("Identifier");

    tree->functionBody->accept(this);
    finishCurrentNodeAsRightSubtree("Body");
}

void TreePrintingVisitor::visitWhileLoopTree(WhileLoopTree *tree) {
    newNode("WHILE");

    tree->expression->accept(this);
    finishCurrentNodeAsLeftSubtree("Expression");

    tree->body->accept(this);
    finishCurrentNodeAsRightSubtree("Body");
}

void TreePrintingVisitor::visitRepeatUntilTree(RepeatUntilTree *tree) {
    newNode("Repeat Until:");

    tree->body->accept(this);
    finishCurrentNodeAsRightSubtree("Body");

    tree->expression->accept(this);
    finishCurrentNodeAsLeftSubtree("Expression");
}

void TreePrintingVisitor::visitIfThenElseStatementTree(IfThenElseStatementTree *tree) {
    newNode("If-Then-Else");
    string label = "if";
    int removeCnt = 0;
    for (auto const &[expr, body]: tree->ifExpThenBlocks) {
        auto node = newNode("");

        if (expr) {
            expr->accept(this);
            finishCurrentNodeAsLeftSubtree("Expression");
        } else {
            label = "Else";
        }

        body->accept(this);
        finishCurrentNodeAsRightSubtree("Body");

        graphAttributes.label(node) = label;

        ++removeCnt;
        label = "Else If";
    }

    while (removeCnt-- > 0) {
        finishCurrentNodeAsRightSubtree();
    }

}

void TreePrintingVisitor::visitProgramTree(ProgramTree *tree) {

    newNode("Program:");
    int finishEls = 0;
    for (auto const &statement: tree->statements) {
        statement->accept(this);
        ++finishEls;
    }

    while (finishEls-- > 0) {
        finishCurrentNodeAsRightSubtree();
    }
}

void TreePrintingVisitor::printTree(const std::string &path, AbstractSyntaxTree *tree) {
    graphAttributes = GraphAttributes(
            graph, GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics
                   | GraphAttributes::nodeLabel | GraphAttributes::nodeStyle
                   | GraphAttributes::edgeStyle | GraphAttributes::edgeLabel
    );

    tree->accept(this);

    auto *ohl = new OptimalHierarchyLayout();
    ohl->layerDistance(50.0);
    ohl->nodeDistance(100.0);
    ohl->weightBalancing(0.8);

    sugiyamaLayout.setRanking(new OptimalRanking);
    sugiyamaLayout.setCrossMin(new MedianHeuristic);
    sugiyamaLayout.setLayout(ohl);
    sugiyamaLayout.call(graphAttributes);

    GraphIO::write(graphAttributes, path, GraphIO::drawSVG);
}


ogdf::node &TreePrintingVisitor::newNode(const std::string_view &label) {
    node n = graph.newNode();
    graphAttributes.label(n) = label;
    graphAttributes.shape(n) = ogdf::Shape::Ellipse;
    graphAttributes.width(n) = 150;
    graphAttributes.height(n) = 150;
    nodes.push(n);
    return nodes.top();
}

void TreePrintingVisitor::finishCurrentNodeAsRightSubtree(const std::string &label) {
    auto rightSubtree = nodes.top();
    nodes.pop();
    auto edge = graph.newEdge(nodes.top(), rightSubtree);
    graphAttributes.label(edge) = label;
    graphAttributes.strokeColor(edge) = {255, 0, 0};
    graphAttributes.strokeWidth(edge) = 5.0f;
}

void TreePrintingVisitor::finishCurrentNodeAsLeftSubtree(const std::string &label) {
    auto rightSubtree = nodes.top();
    nodes.pop();
    auto edge = graph.newEdge(nodes.top(), rightSubtree);
    graphAttributes.label(edge) = label;
    graphAttributes.strokeColor(edge) = {253, 255, 0};
    graphAttributes.strokeWidth(edge) = 5.0f;
}
