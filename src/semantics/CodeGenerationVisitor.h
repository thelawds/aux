//
// Created by miserable on 16.03.2022.
//

#ifndef AUX_CODEGENERATIONVISITOR_H
#define AUX_CODEGENERATIONVISITOR_H

#include "AstVisitor.h"
#include "intermediate_representation/Token.h"
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
#include <stack>
#include <map>

namespace aux::semantics {

    struct CodeGenerationVisitor : AstVisitor {

        CodeGenerationVisitor();

        void visitKeywordTerm(aux::ir::syntax_tree::KeywordTermTree *termTree) override;

        void visitKeywordStatementTree(aux::ir::syntax_tree::KeywordStatementTree *statementTree) override;

        void visitGotoStatementTree(aux::ir::syntax_tree::GotoStatementTree *statementTree) override;

        void visitIdentifierTerm(aux::ir::syntax_tree::IdentifierTermTree *termTree) override;

        void visitIntegerTerm(aux::ir::syntax_tree::IntegerTermTree *termTree) override;

        void visitDoubleTerm(aux::ir::syntax_tree::DoubleTermTree *termTree) override;

        void visitStringLiteralTerm(aux::ir::syntax_tree::StringLiteralTermTree *termTree) override;

        void visitPrefixExpressionTermTree(aux::ir::syntax_tree::PrefixExpressionTermTree *termTree) override;

        void visitExponentTermTree(aux::ir::syntax_tree::ExponentTermTree *termTree) override;

        void visitUnaryTermTree(aux::ir::syntax_tree::UnaryTermTree *termTree) override;

        void visitBinaryExpressionTermTree(aux::ir::syntax_tree::BinaryExpressionTermTree *termTree) override;

        void visitLogicalExpressionTermTree(aux::ir::syntax_tree::LogicalExpressionTermTree *termTree) override;

        void visitTableFieldTermTree(aux::ir::syntax_tree::TableFieldTermTree *termTree) override;

        void visitTableConstructorTermTree(aux::ir::syntax_tree::TableConstructorTermTree *termTree) override;

        void visitExpressionListTree(aux::ir::syntax_tree::ExpressionListTree *listTree) override;

        void visitFunctionCallSuffixTree(aux::ir::syntax_tree::FunctionCallSuffixTree *suffixTree) override;

        void visitTableFieldAccessSuffixTree(aux::ir::syntax_tree::TableFieldAccessSuffixTree *suffixTree) override;

        void visitStructAccessSuffixTree(aux::ir::syntax_tree::StructAccessSuffixTree *suffixTree) override;

        void visitVariableTree(aux::ir::syntax_tree::VariableTree *variableTree) override;

        void visitVariableListTree(aux::ir::syntax_tree::VariableListTree *listTree) override;

        void visitAttributeTree(aux::ir::syntax_tree::AttributeTree *attributeTree) override;

        void visitIdentifierTermListTree(aux::ir::syntax_tree::IdentifierTermListTree *listTree) override;

        void visitParameterListTree(aux::ir::syntax_tree::ParameterListTree *listTree) override;

        void visitAttributeIdentifierListTree(aux::ir::syntax_tree::AttributeIdentifierListTree *listTree) override;

        void visitLabelTree(aux::ir::syntax_tree::LabelTree *labelTree) override;

        void visitFunctionBodyTree(aux::ir::syntax_tree::FunctionBodyTree *bodyTree) override;

        void visitFunctionIdentifierTree(aux::ir::syntax_tree::FunctionIdentifierTree *identifierTree) override;

        void visitAssignmentTree(aux::ir::syntax_tree::AssignmentTree *assignmentTree) override;

        void visitAttributeIdentifierAssignmentTree(aux::ir::syntax_tree::AttributeIdentifierAssignmentTree *assignmentTree) override;

        void visitReturnStatementTree(aux::ir::syntax_tree::ReturnStatementTree *statementTree) override;

        void visitForLoopStatementTree(aux::ir::syntax_tree::ForLoopStatementTree *statementTree) override;

        void visitFunctionDefinitionTree(aux::ir::syntax_tree::FunctionDefinitionTree *definitionTree) override;

        void visitWhileLoopTree(aux::ir::syntax_tree::WhileLoopTree *loopTree) override;

        void visitRepeatUntilTree(aux::ir::syntax_tree::RepeatUntilTree *untilTree) override;

        void visitIfThenElseStatementTree(aux::ir::syntax_tree::IfThenElseStatementTree *ifElseTree) override;

        void visitProgramTree(aux::ir::syntax_tree::ProgramTree *programTree) override;

        void generateLLVMIr(const std::shared_ptr<aux::ir::syntax_tree::ProgramTree>& programTree);

    private:
        enum TosType{
            UNDEFINED,
            INTEGER,
            FLOAT,
            STRING,
            BOOLEAN,
            NIL,
            FUNCTION
        };

        std::string currFuncCallName;

        std::stack<llvm::BasicBlock *> parentBBs;
        std::stack<llvm::Value *> values;
        std::stack<llvm::AllocaInst *> allocations;
        std::stack<TosType> tosTypes;

        llvm::Function *currFunction;
        std::string currentFunctionIdentifier;
        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::IRBuilder<>> builder;
        std::shared_ptr<llvm::Module> module;
        std::map<std::string, std::pair<llvm::Value *, TosType>> allocTable;

        llvm::ConstantPointerNull *getNullValue() const;
        llvm::Value *getBoolValue(ir::tokens::Keyword keyword) const;
        llvm::Value *getIntValue(uint64_t value) const;
        llvm::Value *getFloatValue(long double value) const;
        llvm::Constant *getStringValue(const std::string& value) const;

        llvm::IRBuilder<> *getInCurrentFunctionBuilder();
    };

}


#endif //AUX_CODEGENERATIONVISITOR_H
