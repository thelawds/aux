//
// Created by miserable on 16.03.2022.
//

#ifndef AUX_CODEGENERATIONVISITOR_H
#define AUX_CODEGENERATIONVISITOR_H

#include "Visitor.h"
#include "../intermediate_representation/Token.h"
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

namespace aux::ir::semantics {

    struct CodeGenerationVisitor : Visitor {

        CodeGenerationVisitor();

        void visitKeywordTerm(tree::KeywordTermTree *termTree) override;

        void visitKeywordStatementTree(tree::KeywordStatementTree *statementTree) override;

        void visitGotoStatementTree(tree::GotoStatementTree *statementTree) override;

        void visitIdentifierTerm(tree::IdentifierTermTree *termTree) override;

        void visitIntegerTerm(tree::IntegerTermTree *termTree) override;

        void visitDoubleTerm(tree::DoubleTermTree *termTree) override;

        void visitStringLiteralTerm(tree::StringLiteralTermTree *termTree) override;

        void visitPrefixExpressionTermTree(tree::PrefixExpressionTermTree *termTree) override;

        void visitExponentTermTree(tree::ExponentTermTree *termTree) override;

        void visitUnaryTermTree(tree::UnaryTermTree *termTree) override;

        void visitBinaryExpressionTermTree(tree::BinaryExpressionTermTree *termTree) override;

        void visitLogicalExpressionTermTree(tree::LogicalExpressionTermTree *termTree) override;

        void visitTableFieldTermTree(tree::TableFieldTermTree *termTree) override;

        void visitTableConstructorTermTree(tree::TableConstructorTermTree *termTree) override;

        void visitExpressionListTree(tree::ExpressionListTree *listTree) override;

        void visitFunctionCallSuffixTree(tree::FunctionCallSuffixTree *suffixTree) override;

        void visitTableFieldAccessSuffixTree(tree::TableFieldAccessSuffixTree *suffixTree) override;

        void visitStructAccessSuffixTree(tree::StructAccessSuffixTree *suffixTree) override;

        void visitVariableTree(tree::VariableTree *variableTree) override;

        void visitVariableListTree(tree::VariableListTree *listTree) override;

        void visitAttributeTree(tree::AttributeTree *attributeTree) override;

        void visitIdentifierTermListTree(tree::IdentifierTermListTree *listTree) override;

        void visitParameterListTree(tree::ParameterListTree *listTree) override;

        void visitAttributeIdentifierListTree(tree::AttributeIdentifierListTree *listTree) override;

        void visitLabelTree(tree::LabelTree *labelTree) override;

        void visitFunctionBodyTree(tree::FunctionBodyTree *bodyTree) override;

        void visitFunctionIdentifierTree(tree::FunctionIdentifierTree *identifierTree) override;

        void visitAssignmentTree(tree::AssignmentTree *assignmentTree) override;

        void visitAttributeIdentifierAssignmentTree(tree::AttributeIdentifierAssignmentTree *assignmentTree) override;

        void visitReturnStatementTree(tree::ReturnStatementTree *statementTree) override;

        void visitForLoopStatementTree(tree::ForLoopStatementTree *statementTree) override;

        void visitFunctionDefinitionTree(tree::FunctionDefinitionTree *definitionTree) override;

        void visitWhileLoopTree(tree::WhileLoopTree *loopTree) override;

        void visitRepeatUntilTree(tree::RepeatUntilTree *untilTree) override;

        void visitIfThenElseStatementTree(tree::IfThenElseStatementTree *ifElseTree) override;

        void visitProgramTree(tree::ProgramTree *programTree) override;

        void generateLLVMIr(const std::shared_ptr<tree::ProgramTree>& programTree);

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
        llvm::Value *getBoolValue(tokens::Keyword keyword) const;
        llvm::Value *getIntValue(uint64_t value) const;
        llvm::Value *getFloatValue(long double value) const;
        llvm::Constant *getStringValue(const std::string& value) const;

        llvm::IRBuilder<> *getInCurrentFunctionBuilder();
    };

}


#endif //AUX_CODEGENERATIONVISITOR_H
