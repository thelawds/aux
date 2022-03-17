//
// Created by miserable on 16.03.2022.
//

#include "CodeGenerationVisitor.h"
#include <algorithm>
#include <glog/logging.h>
#include "../intermediate_representation/tree/Tree.h"

using namespace llvm;
using namespace aux::ir::tree;
using namespace aux::ir::tokens;
using namespace aux::ir::semantics;


CodeGenerationVisitor::CodeGenerationVisitor()
        : context(std::make_unique<LLVMContext>()),
          module(std::make_shared<Module>("Test Code Generation", *context)),
          builder(std::make_unique<IRBuilder<>>(*context)) {

    using llvm::Function;

    allocTable["print"] = std::make_pair(getNullValue(), FUNCTION);

    std::vector<Type *> types(1, Type::getDoubleTy(*context));
    FunctionType *functionType = FunctionType::get(Type::getVoidTy(*context), types, false);
    Function *function = Function::Create(functionType, Function::ExternalLinkage, "print", module.get());

    for (auto &arg: function->args()) {
        arg.setName("string");
    }


    allocTable["read"] = std::make_pair(getNullValue(), FUNCTION);

    std::vector<Type *> typesRead(0);
    FunctionType *functionTypeRead = FunctionType::get(Type::getDoubleTy(*context), typesRead, false);
    Function *functionRead = Function::Create(functionTypeRead, Function::ExternalLinkage, "read", module.get());

}

ConstantPointerNull *CodeGenerationVisitor::getNullValue() const {
    return ConstantPointerNull::get(PointerType::getUnqual(*context));
}

Value *CodeGenerationVisitor::getBoolValue(Keyword keyword) const {
    return ConstantFP::get(
            *context, APFloat(
                    keyword == tokens::Keyword::TRUE ? 1.0 : -1.0
            ));
//    return ConstantInt::get(
//            *context,
//            APInt(
//                    1,
//                    keyword == tokens::Keyword::TRUE ? 1 : 0
//            )
//    );
}

Value *CodeGenerationVisitor::getIntValue(uint64_t value) const {
    return getFloatValue(value);
//    return ConstantInt::get(*context, APInt(64, value));
}

Value *CodeGenerationVisitor::getFloatValue(long double value) const {
    return ConstantFP::get(*context, APFloat(double(value)));
}

Constant *CodeGenerationVisitor::getStringValue(const std::string &value) const {
    return ConstantDataArray::getString(*context, value, true);
}

IRBuilder<> *CodeGenerationVisitor::getInCurrentFunctionBuilder() {
    return new IRBuilder<>(parentBBs.top(), parentBBs.top()->end());
}

/**
 *
 * Visiting Tree:
 *
 */

void CodeGenerationVisitor::visitKeywordTerm(KeywordTermTree *termTree) {
    auto &keyword = termTree->keyword;

    if (keyword == Keyword::TRUE || keyword == Keyword::FALSE) {
        values.push(getBoolValue(keyword));
        tosTypes.push(BOOLEAN);
    } else if (keyword == Keyword::NIL) {
        values.push(getNullValue());
        tosTypes.push(NIL);
    } else {
        // TODO: throw exception ?
    }
}

void CodeGenerationVisitor::visitKeywordStatementTree(KeywordStatementTree *statementTree) {}

void CodeGenerationVisitor::visitGotoStatementTree(GotoStatementTree *statementTree) {}

void CodeGenerationVisitor::visitIdentifierTerm(IdentifierTermTree *termTree) {
    std::string name = termTree->value;
    if (!allocTable[name].first) {
        // todo Throw exception
        LOG(FATAL) << "Error: variable " << name << " used before declared";
    }

    if (allocTable[name].second == FUNCTION) {
        currFuncCallName = name;
        return;
    }

    values.push(builder->CreateLoad(allocTable[name].first, name)); // todo: remove deprecated usage
    tosTypes.push(allocTable[name].second);
}

void CodeGenerationVisitor::visitIntegerTerm(IntegerTermTree *termTree) {
    values.push(getIntValue(termTree->value));
    tosTypes.push(INTEGER);
}

void CodeGenerationVisitor::visitDoubleTerm(DoubleTermTree *termTree) {
    values.push(getFloatValue(termTree->value));
    tosTypes.push(FLOAT);
}

void CodeGenerationVisitor::visitStringLiteralTerm(StringLiteralTermTree *termTree) {
    values.push(getStringValue(termTree->value));
    tosTypes.push(STRING);
}

void CodeGenerationVisitor::visitPrefixExpressionTermTree(PrefixExpressionTermTree *termTree) {
    currFuncCallName = "";
    termTree->expressionOrIdentifier->accept(this);
    for (auto &peSuf: termTree->peSuffixTrees) {
        peSuf->accept(this);
    }
}

void CodeGenerationVisitor::visitExponentTermTree(ExponentTermTree *termTree) {
    termTree->left->accept(this); // todo::change
}

void CodeGenerationVisitor::visitUnaryTermTree(UnaryTermTree *termTree) {
    termTree->exponentTermTree->accept(this); // todo: change
}

void CodeGenerationVisitor::visitBinaryExpressionTermTree(BinaryExpressionTermTree *termTree) {
    Value *left = nullptr;
    TosType leftType = UNDEFINED;

    Value *right = nullptr;
    TosType rightType = UNDEFINED;

    if (termTree->left) {
        termTree->left->accept(this);
        leftType = tosTypes.top();
        left = values.top();

        tosTypes.pop();
        values.pop();
    }

    if (termTree->right) {
        termTree->right->accept(this);
        rightType = tosTypes.top();
        right = values.top();

        tosTypes.pop();
        values.pop();
    }

    // todo: change types of Operands!

    if (left && right) {
        switch (termTree->binaryOperator) {
            case aux::ir::tokens::Operator::PLUS:
                values.push(
                        getInCurrentFunctionBuilder()->CreateFAdd(left, right, "addTemp")
                );
                tosTypes.push(FLOAT);
                break;
            case aux::ir::tokens::Operator::MINUS:
                values.push(
                        getInCurrentFunctionBuilder()->CreateFSub(left, right, "subTemp")
                );
                tosTypes.push(FLOAT);
                break;
            case aux::ir::tokens::Operator::ASTERISK:
                values.push(
                        getInCurrentFunctionBuilder()->CreateFMul(left, right, "mulTemp")
                );
                tosTypes.push(FLOAT);
                break;
            case aux::ir::tokens::Operator::SLASH:
                values.push(
                        getInCurrentFunctionBuilder()->CreateFDiv(left, right, "divTemp")
                );
                tosTypes.push(FLOAT);
                break;
            case aux::ir::tokens::Operator::GREATER_THAN:
                values.push(
                        getInCurrentFunctionBuilder()->CreateFCmpUGT(left, right, "cmpTemp")
                );
                tosTypes.push(BOOLEAN);
                break;
            case aux::ir::tokens::Operator::LESS_THAN:
                values.push(
                        getInCurrentFunctionBuilder()->CreateFCmpULT(left, right, "cmpTemp")
                );
                tosTypes.push(BOOLEAN);
                break;
            case aux::ir::tokens::Operator::GT_EQUAL:
                values.push(
                        getInCurrentFunctionBuilder()->CreateFCmpUGE(left, right, "cmpTemp")
                );
                tosTypes.push(BOOLEAN);
                break;
            case aux::ir::tokens::Operator::LT_EQUAL:
                values.push(
                        getInCurrentFunctionBuilder()->CreateFCmpULE(left, right, "cmpTemp")
                );
                tosTypes.push(BOOLEAN);
                break;
            case aux::ir::tokens::Operator::EQUAL_EQUAL:
                values.push(
                        getInCurrentFunctionBuilder()->CreateFCmpUEQ(left, right, "cmpTemp")
                );
                tosTypes.push(BOOLEAN);
                break;
            case aux::ir::tokens::Operator::TILDA_EQUAL:
                values.push(
                        getInCurrentFunctionBuilder()->CreateFCmpUNE(left, right, "cmpTemp")
                );
                tosTypes.push(BOOLEAN);
                break;
                // todo: add % operator
            default:
                return;
                //todo throw error message
        }
    }
}

void CodeGenerationVisitor::visitLogicalExpressionTermTree(LogicalExpressionTermTree *termTree) {

}

void CodeGenerationVisitor::visitTableFieldTermTree(TableFieldTermTree *termTree) {

}

void CodeGenerationVisitor::visitTableConstructorTermTree(TableConstructorTermTree *termTree) {

}

void CodeGenerationVisitor::visitExpressionListTree(ExpressionListTree *listTree) {
    std::for_each(
            listTree->expressions.begin(),
            listTree->expressions.end(),
            [this](std::shared_ptr<ExpressionTree> &expr) {
                expr->accept(this);
            }
    );
}

void CodeGenerationVisitor::visitFunctionCallSuffixTree(FunctionCallSuffixTree *suffixTree) {
    if (currFuncCallName.empty()) {
        return;
    }

    llvm::Function *callee = module->getFunction(currFuncCallName);

    if (!callee) {
        LOG(FATAL) << "UNKNOWN FUNCTION " << currFuncCallName << " REFERENCED";
    }

    std::vector<Value *> args;
    if (std::dynamic_pointer_cast<StringLiteralTermTree>(suffixTree->arguments)) {
        suffixTree->arguments->accept(this);
        args.push_back(values.top());
        values.pop();
        tosTypes.pop();
    } else {
        auto exprListTree = std::static_pointer_cast<ExpressionListTree>(suffixTree->arguments)->expressions;
        for (auto &arg: exprListTree) {
            arg->accept(this);
            args.push_back(values.top());
            values.pop();
            tosTypes.pop();
        }
    }

    values.push(builder->CreateCall(callee, args));
    tosTypes.push(FLOAT);
}

void CodeGenerationVisitor::visitTableFieldAccessSuffixTree(TableFieldAccessSuffixTree *suffixTree) {

}

void CodeGenerationVisitor::visitStructAccessSuffixTree(StructAccessSuffixTree *suffixTree) {

}

void CodeGenerationVisitor::visitVariableTree(VariableTree *variableTree) {
    std::string name = variableTree->identifier->value; // todo
    if (!allocTable[name].first) {
        allocTable[name] = std::make_pair(getNullValue(), NIL);
    }

    values.push(allocTable[name].first);
    tosTypes.push(allocTable[name].second);
}

void CodeGenerationVisitor::visitVariableListTree(VariableListTree *listTree) {
    for_each(
            listTree->variableTrees.begin(),
            listTree->variableTrees.end(),
            [this](std::shared_ptr<VariableTree> &variableTree) {
                variableTree->accept(this);
            }
    );
}

void CodeGenerationVisitor::visitAttributeTree(AttributeTree *attributeTree) {

}

void CodeGenerationVisitor::visitIdentifierTermListTree(IdentifierTermListTree *listTree) {

}

void CodeGenerationVisitor::visitParameterListTree(ParameterListTree *listTree) {

}

void CodeGenerationVisitor::visitAttributeIdentifierListTree(AttributeIdentifierListTree *listTree) {

}

void CodeGenerationVisitor::visitLabelTree(LabelTree *labelTree) {

}

void CodeGenerationVisitor::visitFunctionIdentifierTree(FunctionIdentifierTree *identifierTree) {
    currentFunctionIdentifier = identifierTree->primaryIdentifier->getValue();
    for (auto &secId: identifierTree->secondaryIdentifiers) {
        currentFunctionIdentifier += secId->getValue();
    }

    if (identifierTree->colonIdentifier) {
        currentFunctionIdentifier += identifierTree->colonIdentifier->getValue();
    }
}

void CodeGenerationVisitor::visitFunctionBodyTree(FunctionBodyTree *bodyTree) {
    using llvm::Function;
    std::string functionName = currentFunctionIdentifier;

    // todo: make it accept any value and accept varargs
    std::vector<Type *> types(bodyTree->parameters.size(), Type::getDoubleTy(*context));
    FunctionType *functionType = FunctionType::get(Type::getDoubleTy(*context), types, false);
    Function *function = Function::Create(functionType, Function::ExternalLinkage, functionName, module.get());

    size_t idx = 0;
    for (auto &arg: function->args()) {
        arg.setName(bodyTree->parameters[idx++]->value);
    }

    allocTable[functionName] = std::make_pair(function, FUNCTION);

    BasicBlock *block = BasicBlock::Create(*context, "Entry", function);
    builder->SetInsertPoint(block);

    for (auto &arg: function->args()) {
        // todo: allocate mem here
        allocations.push(builder->CreateAlloca(Type::getDoubleTy(*context), nullptr, arg.getName().str()));
        allocTable[arg.getName().str()] = std::make_pair(allocations.top(), FLOAT);
        builder->CreateStore(&arg, allocations.top());
        tosTypes.push(FLOAT);

//        allocTable[arg.getName().str()] = std::make_pair(&arg, FLOAT);
    }

    Function *parent = currFunction;
    currFunction = function;
    parentBBs.push(block);
    bodyTree->body->accept(this);
    parentBBs.pop();
    currFunction = parent;

    auto &statements = bodyTree->body->statements;
    if (
            statements.empty()
            || !std::dynamic_pointer_cast<ReturnStatementTree>(statements[statements.size() - 1])
               && !std::dynamic_pointer_cast<IfThenElseStatementTree>(statements[statements.size() - 1])
    ) {
        builder->CreateRet(getFloatValue(0));
    }

    verifyFunction(*function, &errs());

    if (!parentBBs.empty()) {
        builder->SetInsertPoint(parentBBs.top());
    }

    values.push(function);
    tosTypes.push(FUNCTION);
}

void CodeGenerationVisitor::visitFunctionDefinitionTree(FunctionDefinitionTree *definitionTree) {
    currentFunctionIdentifier = "";
    definitionTree->identifier->accept(this);
    definitionTree->functionBody->accept(this);
}

void CodeGenerationVisitor::visitAssignmentTree(AssignmentTree *assignmentTree) {
    auto &variables = assignmentTree->variableList->variableTrees;
    auto &expressions = assignmentTree->expressionList->expressions;

    auto var = variables.begin();
    auto exp = expressions.begin();
    for (; var < variables.end() && exp < expressions.end(); ++var, ++exp) {
        (*exp)->accept(this);
        auto &varName = (*var)->identifier->value;
        auto currBuilder = getInCurrentFunctionBuilder();

        if (allocTable[varName].first) {
            currBuilder->CreateStore(values.top(), allocTable[varName].first);
        } else {
            Type *type;
            switch (tosTypes.top()) {
                case INTEGER:
                    type = Type::getDoubleTy(*context); // todo: change for int64 type ?
                    break;
                case FLOAT:
                    type = Type::getDoubleTy(*context);
                    break;
                case STRING:
                    type = Type::getInt8PtrTy(*context);
                    break;
                case BOOLEAN:
                    type = Type::getDoubleTy(*context); // todo: change for int1 type?
                    break;
                    //todo add NIL
            }

            allocations.push(currBuilder->CreateAlloca(type, nullptr, varName));
            currBuilder->CreateStore(values.top(), allocations.top());
            allocTable[varName] = std::make_pair(allocations.top(), tosTypes.top());
        }

        tosTypes.pop();
        values.pop();
    }

    while (var < variables.end()) {
        auto &name = (*var)->identifier->value;
        allocations.push(getInCurrentFunctionBuilder()->CreateAlloca(Type::getDoublePtrTy(*context), nullptr, name));
        tosTypes.push(FLOAT);

        allocTable[name] = std::make_pair(allocations.top(), FLOAT);
        getInCurrentFunctionBuilder()->CreateStore(getFloatValue(0.), allocTable[name].first);

        ++var;
    }

}

void CodeGenerationVisitor::visitAttributeIdentifierAssignmentTree(AttributeIdentifierAssignmentTree *assignmentTree) {

}

void CodeGenerationVisitor::visitReturnStatementTree(ReturnStatementTree *statementTree) {
    auto &exprs = statementTree->expressions->expressions;
    if (exprs.empty()) {
        getInCurrentFunctionBuilder()->CreateRet(getFloatValue(0));
    } else {
        exprs[0]->accept(this); // todo now return only first value, need to return all
        getInCurrentFunctionBuilder()->CreateRet(values.top());
        values.pop();
        tosTypes.pop();
    }
}

void CodeGenerationVisitor::visitForLoopStatementTree(ForLoopStatementTree *statementTree) {

}

void CodeGenerationVisitor::visitWhileLoopTree(WhileLoopTree *loopTree) {

}

void CodeGenerationVisitor::visitRepeatUntilTree(RepeatUntilTree *untilTree) {

}

void CodeGenerationVisitor::visitIfThenElseStatementTree(IfThenElseStatementTree *ifElseTree) {
    auto *ifThen = &ifElseTree->ifExpThenBlocks[0];
    auto *elseThen = &ifElseTree->ifExpThenBlocks[0];

    if (ifElseTree->ifExpThenBlocks.size() > 1) {
        elseThen = &ifElseTree->ifExpThenBlocks[1];
    } else {
        elseThen = nullptr;
    }

    ifThen->first->accept(this);
    auto condition = values.top();

    BasicBlock *thenBlock = BasicBlock::Create(*context, "then", currFunction);
    BasicBlock *elseBlock = BasicBlock::Create(*context, "else", currFunction);
    BasicBlock *mergeBlock = BasicBlock::Create(*context, "afterIfElse", currFunction);

    getInCurrentFunctionBuilder()->CreateCondBr(condition, thenBlock, elseBlock);

    parentBBs.push(thenBlock);
    builder->SetInsertPoint(thenBlock);
    ifThen->second->accept(this);
    currFunction->getBasicBlockList().push_back(thenBlock);
    parentBBs.pop();
//    builder->CreateBr(mergeBlock);

    // todo: не будет работать, если после ифа есть что-то еще!

    if (elseThen) {
        builder->SetInsertPoint(elseBlock);
        parentBBs.push(elseBlock);
        elseThen->second->accept(this);
        currFunction->getBasicBlockList().push_back(elseBlock);
        parentBBs.pop();
//        builder->CreateBr(mergeBlock);
    }

}

void CodeGenerationVisitor::visitProgramTree(ProgramTree *programTree) {
    for_each(
            programTree->statements.begin(),
            programTree->statements.end(),
            [this](std::shared_ptr<StatementTree> &tree) {
                tree->accept(this);
            }
    );
}

void CodeGenerationVisitor::generateLLVMIr(const std::shared_ptr<ProgramTree> &programTree) {
    auto mainBody = std::make_shared<FunctionBodyTree>(programTree);
    auto main = std::make_unique<FunctionDefinitionTree>(
            std::make_shared<FunctionIdentifierTree>(
                    std::make_shared<TokenIdentifier>("main")
            ),
            mainBody
    );
    main->accept(this);

    module->print(outs(), nullptr, false, true);

    // todo: generate output code:

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

    auto filename = "MyProgram.o";
    std::error_code errorCode;
    raw_fd_ostream dest(filename, errorCode, sys::fs::OF_None);

    legacy::PassManager passManager;
    auto fileType = CGFT_ObjectFile;

    if (targetMachine->addPassesToEmitFile(passManager, dest, nullptr, fileType)) {
        LOG(FATAL) << "TargetMachine can't emit a file of this type";
    }

    passManager.run(*module);
    dest.flush();
}

