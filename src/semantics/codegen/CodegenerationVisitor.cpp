//
// Created by miserable on 20.03.2022.
//

#include "CodeGenerationVisitor.h"

#include <glog/logging.h>
#include <fstream>
#include <map>
#include <utility>

using namespace llvm;
using namespace aux::ir::program_tree;
using namespace aux::semantics::codegen;
using namespace aux::ir::program_tree::statement;
using namespace aux::ir::program_tree::expression;

struct FunctionDefinition {
    std::string name;
    Type *returnType;
    std::vector<Type *> argTypes;

    FunctionDefinition(std::string name, Type *returnType, std::vector<Type *> argTypes)
            : returnType(returnType), name(std::move(name)), argTypes(std::move(argTypes)) {}

};

CodeGenerationVisitor::CodeGenerationVisitor()
        : context(std::make_unique<LLVMContext>()),
          builder(std::make_unique<IRBuilder<>>(*context)),
          module(std::make_unique<Module>("Test lua codegen", *context)) // todo: rename with file name
{
    StructType::create(*context, {Type::getInt32Ty(*context), Type::getInt8PtrTy(*context)}, "T");

    std::vector<FunctionDefinition> stdlibFunctionDefinitions = {
            {"__getTable__",      getTypeT(),                   {}},
            {"__getTableField__", getTypeT()->getPointerTo(),   {getTypeT()->getPointerTo(), getTypeT()}},
            {"__putField__",      Type::getVoidTy(*context),    {getTypeT(),                 getTypeT(), getTypeT()}},
            {"__getBoolean__",    getTypeT(),                   {Type::getInt1Ty(*context)}},
            {"__getInteger__",    getTypeT(),                   {Type::getInt64Ty(*context)}},
            {"__getFloat__",      getTypeT(),                   {Type::getDoubleTy(*context)}},
            {"__getString__",     getTypeT(),                   {Type::getInt8PtrTy(*context)}},
            {"__toFloat__",       Type::getDoubleTy(*context),  {getTypeT()}},
            {"__toString__",      Type::getInt8PtrTy(*context), {getTypeT()}},
            {"__print__",         Type::getVoidTy(*context),    {getTypeT()}},
            {"__read__",          getTypeT(),                   {}}
    };

    for (auto &[name, returnType, argTypes]: stdlibFunctionDefinitions) {
        Function::Create(
                FunctionType::get(returnType, argTypes, false),
                Function::ExternalLinkage, name, module.get()
        );
    }

}

void CodeGenerationVisitor::generateLLVMIr(const std::shared_ptr<ProgramTree> &programTree) {
    auto *functionType = FunctionType::get(Type::getVoidTy(*context), {}, false);
    auto *function = Function::Create(functionType, Function::ExternalLinkage, "__start__", module.get());

    // todo: now this is for testing purposes, change it after creating FunctionDefinitionVisitor
    auto *basicBlock = BasicBlock::Create(*context, "begin", function);
    builder->SetInsertPoint(basicBlock);
    programTree->accept(this);

    for (auto[name, allocaInst]: symbolTable.getInternal()) {
        builder->CreateCall(
                module->getFunction("__print__"), {
                        visit(
                                std::make_shared<aux::ir::program_tree::expression::ExpressionTerm>(
                                        name + ": ",
                                        aux::ir::program_tree::expression::ExpressionTerm::STRING_LITERAL
                                )
                        )
                }
        );

        builder->CreateCall(
                module->getFunction("__print__"), {
                        builder->CreateLoad(
                                getTypeT(), allocaInst,
                                "printingValueTemp_" + name
                        )
                }
        );

        builder->CreateCall(
                module->getFunction("__print__"), {
                        visit(
                                std::make_shared<aux::ir::program_tree::expression::ExpressionTerm>(
                                        "\n",
                                        aux::ir::program_tree::expression::ExpressionTerm::STRING_LITERAL
                                )
                        )
                }
        );

    }

    builder->CreateRetVoid();
    verifyFunction(*function, &errs());

    std::error_code errorCode;
    raw_fd_ostream out(
            "/home/miserable/Documents/Compiler Construction/aux/cmake-build-debug/SimpleCodeGen.ll",
            errorCode
    );
    module->print(outs(), nullptr, false, true);
    module->print(out, nullptr, false, true);

    // todo: code generation

    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto targetTriple = sys::getDefaultTargetTriple();
    module->setTargetTriple(targetTriple);
    std::string error;

    auto target = TargetRegistry::lookupTarget(targetTriple, error);

    auto cpu = "generic";
    auto features = "";

    TargetOptions opt;
    auto rModel = Optional<Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rModel);

    module->setDataLayout(targetMachine->createDataLayout());

    auto filename = "Codegen.o";
    raw_fd_ostream dest(filename, errorCode, sys::fs::OF_None);

    legacy::PassManager passManager;
    auto fileType = CGFT_ObjectFile;

    if (targetMachine->addPassesToEmitFile(passManager, dest, nullptr, fileType)) {
        LOG(FATAL) << "TargetMachine can't emit a file of this type";
    }

    passManager.run(*module);
    dest.flush();

}

Value *CodeGenerationVisitor::stackPop() {
    auto result = returnValues.top();
    returnValues.pop();
    return result;
}

void CodeGenerationVisitor::stackPush(Value *value) {
    returnValues.push(value);
}

Value *CodeGenerationVisitor::visit(const std::shared_ptr<ProgramTree> &abstractSyntaxTree) {
    abstractSyntaxTree->accept(this);
    return stackPop();
}

llvm::StructType *CodeGenerationVisitor::getTypeT() {
    return StructType::getTypeByName(*context, "T");
}


