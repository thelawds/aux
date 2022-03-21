
#include "CodeGenerationVisitor.h"
#include "intermediate_representation/tree/ProgramTree.h"

#include <glog/logging.h>

using namespace llvm;
using namespace aux::ir::program_tree;
using namespace aux::semantics::codegen;
using namespace aux::ir::program_tree::statement;
using namespace aux::ir::program_tree::expression;

void CodeGenerationVisitor::visitNilTerm(NilTerm *term) {
    stackPush(
            ConstantStruct::get(
                    getTypeT(),
                    ConstantInt::get(*context, APInt(32, 4)),
                    ConstantPointerNull::get(Type::getInt8PtrTy(*context))
            )
    );
}

void CodeGenerationVisitor::visitExpressionTerm(ExpressionTerm *term) {
    std::string &value = term->value;

    switch (term->termType) {
        case ExpressionTerm::IDENTIFIER: {
            if (symbolTable.contains(value)) {
                stackPush(
                        builder->CreateLoad(
                                getTypeT(),
                                symbolTable.find(value)
                        )
                );
            } else {
                LOG(FATAL) << "Variable " + value + " is undefined"; // TODO
            }
            break;
        }
        case ir::program_tree::expression::ExpressionTerm::BOOLEAN: {
            stackPush(
                    builder->CreateCall(
                            module->getFunction("__getBoolean__"),
                            ConstantInt::get(Type::getInt1Ty(*context), APInt(1, value == "1"))
                    )
            );
            break;
        }
        case ir::program_tree::expression::ExpressionTerm::INTEGER: {
            stackPush(
                    builder->CreateCall(
                            module->getFunction("__getInteger__"),
                            ConstantInt::get(Type::getInt64Ty(*context), APInt(64, std::stol(value)))
                    )
            );
            break;
        }
        case ir::program_tree::expression::ExpressionTerm::FLOAT: {
            stackPush(
                    builder->CreateCall(
                            module->getFunction("__getFloat__"),
                            ConstantFP::get(Type::getDoubleTy(*context), APFloat(std::stod(value)))
                    )
            );
            break;
        }
        case ir::program_tree::expression::ExpressionTerm::STRING_LITERAL: {
            auto *strValue = ConstantDataArray::getString(*context, value, true);
            auto *val = builder->CreateAlloca(strValue->getType());
            builder->CreateStore(strValue, val);

            auto *gep = builder->CreateInBoundsGEP(
                    val->getType()->getScalarType()->getPointerElementType(),
                    val,
                    {
                            ConstantInt::getIntegerValue(Type::getInt64Ty(*context), APInt(64, 0)),
                            ConstantInt::getIntegerValue(Type::getInt64Ty(*context), APInt(64, 0))
                    }
            );

            stackPush(builder->CreateCall(module->getFunction("__getString__"), gep));
            break;
        }
    }

}

void CodeGenerationVisitor::visitArithmeticExpression(ArithmeticExpression *expression) {
    auto *leftOperand = builder->CreateCall(
            module->getFunction("__toFloat__"), visit(expression->left)
    );
    auto *rightOperand = builder->CreateCall(
            module->getFunction("__toFloat__"), visit(expression->right)
    );

    Value *result;
    switch (expression->operation) {
        case ir::program_tree::expression::ArithmeticExpression::ADDITION:
            result = builder->CreateFAdd(leftOperand, rightOperand);
            break;
        case ir::program_tree::expression::ArithmeticExpression::SUBTRACTION:
            result = builder->CreateFSub(leftOperand, rightOperand);
            break;
        case ir::program_tree::expression::ArithmeticExpression::MULTIPLICATION:
            result = builder->CreateFMul(leftOperand, rightOperand);
            break;
        case ir::program_tree::expression::ArithmeticExpression::DIVISION:
            result = builder->CreateFDiv(leftOperand, rightOperand);
            break;
        // todo: implement
        case ir::program_tree::expression::ArithmeticExpression::FLOOR_DIVISION:
            break;
        case ir::program_tree::expression::ArithmeticExpression::MODULO:
            break;
        case ir::program_tree::expression::ArithmeticExpression::EXPONENTIATION:
            break;
    }

    stackPush(builder->CreateCall(module->getFunction("__getFloat__"), result));
}
