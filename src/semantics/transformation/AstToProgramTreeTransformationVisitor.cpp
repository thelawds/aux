//
// Created by miserable on 19.03.2022.
//

#include "AstToProgramTreeTransformationVisitor.h"
#include "intermediate_representation/tree/SyntaxTree.h"

using namespace aux::semantics;
using namespace aux::ir::syntax_tree;


void AstToProgramTreeTransformationVisitor::visitLabelTree(LabelTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitGotoStatementTree(GotoStatementTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitKeywordStatementTree(KeywordStatementTree *tree) {

}


void AstToProgramTreeTransformationVisitor::visitExpressionListTree(ExpressionListTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitIdentifierTermListTree(IdentifierTermListTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitReturnStatementTree(ReturnStatementTree *tree) {

}

void AstToProgramTreeTransformationVisitor::visitProgramTree(ProgramTree *tree) {
    auto chunkTree = std::make_shared<ir::program_tree::statement::ChunkTree>();

    for (auto &statement: tree->statements) {
        chunkTree->append(
                std::dynamic_pointer_cast<aux::ir::program_tree::StatementTree>(
                        visit(statement)
                )
        );
    }

    stackPush(chunkTree);
}

/**
 * @Miscellaneous
 */

void AstToProgramTreeTransformationVisitor::stackPush(
        const std::shared_ptr<aux::ir::program_tree::ProgramTree> &value
) {
    returnValues.push(value);
}

std::shared_ptr<aux::ir::program_tree::ProgramTree> AstToProgramTreeTransformationVisitor::stackPop() {
    auto returnValue = returnValues.top();
    returnValues.pop();
    return returnValue;
}

std::shared_ptr<aux::ir::program_tree::ProgramTree> AstToProgramTreeTransformationVisitor::visit(
        const std::shared_ptr<AbstractSyntaxTree>& abstractSyntaxTree
) {
    abstractSyntaxTree->accept(this);
    return stackPop();
}

std::shared_ptr<aux::ir::program_tree::ProgramTree> AstToProgramTreeTransformationVisitor::transform(
        aux::ir::syntax_tree::AbstractSyntaxTree *abstractSyntaxTree
) {
    abstractSyntaxTree->accept(this);
    return stackPop();
}
