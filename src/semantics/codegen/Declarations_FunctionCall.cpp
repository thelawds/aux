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

void CodeGenerationVisitor::visitIdentifierReferenceTree(statement::IdentifierReferenceTree *tree) {
    if (symbolTable.contains(tree->identifier)) {
        stackPush(symbolTable.find(tree->identifier));
    } else {
        auto *allocation = builder->CreateAlloca(getTypeT(), 0, nullptr, tree->identifier);
        symbolTable.add(tree->identifier, allocation);
        stackPush(allocation);
    }
}

void CodeGenerationVisitor::visitTableReferenceTree(aux::ir::program_tree::statement::TableReferenceTree *tree) {
    auto tableReference = visit(tree->table);
    auto expression = visit(tree->expression);
    stackPush(
            builder->CreateCall(
                    module->getFunction("__getTableFieldPtr__"),
                    {tableReference, expression}
            )
    );
}

void CodeGenerationVisitor::visitAssignmentStatement(AssignmentStatement *assignmentStatement) {
    std::vector<std::pair<Value *, Value *>> resultAssignments;
    for (auto &[variable, expression]: assignmentStatement->assignments) {
        auto *variableReference = visit(variable);
        auto *value = visit(expression);

        if (std::dynamic_pointer_cast<aux::ir::program_tree::expression::TableConstructorTerm>(expression)) {
            builder->CreateStore(value, variableReference);
        } else {
            resultAssignments.emplace_back(variableReference, value);
        }

    }

    for (auto &[variableReference, value]: resultAssignments) {
        builder->CreateStore(value, variableReference);
    }
}
