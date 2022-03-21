#include "CodeGenerationVisitor.h"

using namespace llvm;
using namespace aux::ir::program_tree;
using namespace aux::semantics::codegen;
using namespace aux::ir::program_tree::statement;
using namespace aux::ir::program_tree::expression;

void CodeGenerationVisitor::visitChunkTree(statement::ChunkTree *tree) {
    for (auto &statement: tree->statements) {
        statement->accept(this);
    }
}

void CodeGenerationVisitor::visitIdentifierVariableReferenceTree(IdentifierVariableReferenceTree *tree) {
    if (symbolTable.contains(tree->identifier)) {
        stackPush(symbolTable.find(tree->identifier));
    } else {
        auto *allocation = builder->CreateAlloca(getTypeT(), 0, nullptr, tree->identifier);
        symbolTable.add(tree->identifier, allocation);
        stackPush(allocation);
    }
}

void CodeGenerationVisitor::visitAssignmentStatement(AssignmentStatement *assignmentStatement) {
    for (auto &[variable, expression]: assignmentStatement->assignments) {
        auto *allocation = (AllocaInst *) visit(variable);
        auto *value = visit(expression);
        builder->CreateStore(value, allocation);
        builder->CreateCall(
                module->getFunction("__print__"), {
                        builder->CreateLoad(
                                getTypeT(), allocation,
                                "printingValueTemp_" +
                                        std::dynamic_pointer_cast<IdentifierVariableReferenceTree>(variable)->identifier
                        )
                }
        );
    }
}
