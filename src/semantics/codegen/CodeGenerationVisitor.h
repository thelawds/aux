
#ifndef AUX_CODEGENERATIONVISITOR_H
#define AUX_CODEGENERATIONVISITOR_H

#include "semantics/ProgramTreeVisitor.h"
#include "intermediate_representation/SymbolTable.h"
#include "intermediate_representation/tree/ProgramTree.h"

#include <stack>

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/TargetRegistry.h>


namespace aux::semantics::codegen {

    struct CodeGenerationVisitor : ProgramTreeVisitor {

        CodeGenerationVisitor();

        void generateLLVMIr(const std::shared_ptr<ir::program_tree::ProgramTree>& programTree);

        /**
         * @Expressions
         */

        void visitNilTerm(aux::ir::program_tree::expression::NilTerm *term) override;

        void visitExpressionTerm(aux::ir::program_tree::expression::ExpressionTerm *term) override;

        void visitTableConstructorTerm(aux::ir::program_tree::expression::TableConstructorTerm *term) override;

        void visitTableAccessTerm(aux::ir::program_tree::expression::TableAccessTerm *term) override;

        void visitArithmeticExpression(aux::ir::program_tree::expression::ArithmeticExpression *expression) override;

        /**
         * @Statements
         */

        void visitChunkTree(aux::ir::program_tree::statement::ChunkTree *tree) override;

        void visitIdentifierReferenceTree(aux::ir::program_tree::statement::IdentifierReferenceTree *tree) override;

        void visitTableReferenceTree(aux::ir::program_tree::statement::TableReferenceTree *tree) override;

        void visitAssignmentStatement(aux::ir::program_tree::statement::AssignmentStatement *assignmentStatement) override;

    private:
        std::stack<llvm::Value *> returnValues;
        aux::ir::SymbolTable<llvm::AllocaInst *> symbolTable;

        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::IRBuilder<>> builder;
        std::unique_ptr<llvm::Module> module;


        llvm::Value *stackPop();
        void stackPush(llvm::Value *value);
        llvm::Value *visit(const std::shared_ptr<aux::ir::program_tree::ProgramTree> &abstractSyntaxTree);

        llvm::StructType* getTypeT();

    };

}

#endif //AUX_CODEGENERATIONVISITOR_H
