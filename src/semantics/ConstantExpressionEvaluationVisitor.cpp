//
// Created by miserable on 12.03.2022.
//

#include "ConstantExpressionEvaluationVisitor.h"
#include "../intermediate_representation/tree/Tree.h"
#include <unordered_set>
#include <cmath>

using namespace aux::ir::tree;
using namespace aux::ir::semantics;

void ConstantExpressionEvaluationVisitor::visitKeywordStatementTree(KeywordStatementTree *statementTree) {}
void ConstantExpressionEvaluationVisitor::visitGotoStatementTree(GotoStatementTree *statementTree) {}
void ConstantExpressionEvaluationVisitor::visitAttributeTree(AttributeTree *attributeTree) {}
void ConstantExpressionEvaluationVisitor::visitIdentifierTermListTree(IdentifierTermListTree *listTree) {}
void ConstantExpressionEvaluationVisitor::visitParameterListTree(ParameterListTree *listTree) {}
void ConstantExpressionEvaluationVisitor::visitAttributeIdentifierListTree(AttributeIdentifierListTree *listTree) {}
void ConstantExpressionEvaluationVisitor::visitLabelTree(LabelTree *labelTree) {}
void ConstantExpressionEvaluationVisitor::visitFunctionIdentifierTree(FunctionIdentifierTree *identifierTree) {}
void ConstantExpressionEvaluationVisitor::visitIdentifierTerm(IdentifierTermTree *termTree) {}
void ConstantExpressionEvaluationVisitor::visitStructAccessSuffixTree(StructAccessSuffixTree *suffixTree) {}

void ConstantExpressionEvaluationVisitor::visitKeywordTerm(KeywordTermTree *termTree) {
    auto &keyword = termTree->keyword;
    if (keyword == tokens::Keyword::TRUE || keyword == tokens::Keyword::FALSE) {
        booleanValue = keyword == tokens::Keyword::TRUE;
        availableValue = AvailableValue::BOOLEAN;
    }
}

void ConstantExpressionEvaluationVisitor::visitIntegerTerm(IntegerTermTree *termTree) {
    integerValue = termTree->value;
    availableValue = AvailableValue::INTEGER;
}

void ConstantExpressionEvaluationVisitor::visitDoubleTerm(DoubleTermTree *termTree) {
    doubleValue = termTree->value;
    availableValue = AvailableValue::DOUBLE;
}

void ConstantExpressionEvaluationVisitor::visitStringLiteralTerm(StringLiteralTermTree *termTree) {
    stringValue = termTree->value;
    availableValue = AvailableValue::STRING;
}

void ConstantExpressionEvaluationVisitor::visitPrefixExpressionTermTree(PrefixExpressionTermTree *termTree) {
    termTree->expressionOrIdentifier->accept(this);
    for (auto &peSuffix: termTree->peSuffixTrees) {
        peSuffix->accept(this);
    }
}

void ConstantExpressionEvaluationVisitor::visitExponentTermTree(ExponentTermTree *termTree) {
    availableValue = AvailableValue::NONE;

    termTree->left->accept(this);
    if (isAvailable()) {
        long double left = getAsDouble();

        if (!termTree->right) {
            availableValue = AvailableValue::DOUBLE;
            doubleValue = left;
            return;
        }

        availableValue = AvailableValue::NONE;
        termTree->right->accept(this);
        if (isAvailable()) {
            long double right = getAsDouble();
            doubleValue = std::pow(left, right);
            availableValue = AvailableValue::DOUBLE;
            return;
        }

    }
}

void ConstantExpressionEvaluationVisitor::visitUnaryTermTree(UnaryTermTree *termTree) {
    availableValue = AvailableValue::NONE;

    termTree->exponentTermTree->accept(this);
    if (isAvailable()) {
        switch (availableValue) {
            case AvailableValue::STRING:
                termTree->exponentTermTree = std::make_shared<StringLiteralTermTree>(getAsString());
                break;
            case AvailableValue::INTEGER:
                termTree->exponentTermTree = std::make_shared<IntegerTermTree>(getAsInteger());
                break;
            case AvailableValue::DOUBLE:
                termTree->exponentTermTree = std::make_shared<DoubleTermTree>(getAsDouble());
                break;
            case AvailableValue::BOOLEAN:
                termTree->exponentTermTree = std::make_shared<KeywordTermTree>(
                        getAsBoolean() ? Keyword::TRUE : Keyword::FALSE
                );
                break;
            case AvailableValue::NONE:
                break;
        }

        if (termTree->isKeywordOperator) {
            auto keyword = termTree->unaryTermOperator.keyword;
            if (keyword == tokens::Keyword::NOT) {
                booleanValue = !getAsBoolean();
                availableValue = AvailableValue::BOOLEAN;
            }
        } else {
            auto op = termTree->unaryTermOperator.unaryOperator;
            if (op == tokens::Operator::SHARP) {
                integerValue = getAsString().size();
                availableValue = AvailableValue::INTEGER;
            } else if (op == tokens::Operator::MINUS) {
                doubleValue = -getAsDouble();
                availableValue = AvailableValue::DOUBLE;
            } else if (op == tokens::Operator::TILDA) {
                integerValue = ~getAsInteger();
                availableValue = AvailableValue::INTEGER;
            }
        }
    }
}

void ConstantExpressionEvaluationVisitor::visitBinaryExpressionTermTree(BinaryExpressionTermTree *termTree) {
    static std::unordered_set<Operator> bitwiseOperators = {
            Operator::AMPERSAND, tokens::Operator::VERTICAL_BAR, Operator::TILDA, Operator::GT_GT, Operator::LT_LT
    };
    static std::unordered_set<Operator> relationalOperators = {
            Operator::EQUAL_EQUAL, Operator::TILDA_EQUAL, Operator::LESS_THAN, Operator::GREATER_THAN,
            Operator::LT_EQUAL, Operator::GT_EQUAL
    };

    availableValue = AvailableValue::NONE;

    bool isStringConcat = termTree->binaryOperator == tokens::Operator::DOT_DOT;
    bool isBitwiseOperator = bitwiseOperators.contains(termTree->binaryOperator);
    bool isRelationalOperator = relationalOperators.contains(termTree->binaryOperator);
    bool leftFinal = false;
    bool rightFinal = false;

    termTree->left->accept(this);
    if (isAvailable()) {
        leftFinal = true;
        if (isStringConcat) {
            termTree->left = std::make_shared<StringLiteralTermTree>(getAsString());
        } else if (isBitwiseOperator) {
            termTree->left = std::make_shared<IntegerTermTree>(getAsInteger());
        } else if (isRelationalOperator) {
            termTree->left = getAsIs();
        } else {
            termTree->left = std::make_shared<DoubleTermTree>(getAsDouble());
        }
    }

    availableValue = AvailableValue::NONE;
    termTree->right->accept(this);
    if (isAvailable()) {
        rightFinal = true;
        if (isRelationalOperator) {

        }
        if (isStringConcat) {
            termTree->right = std::make_shared<StringLiteralTermTree>(getAsString());
        } else if (isBitwiseOperator) {
            termTree->right = std::make_shared<IntegerTermTree>(getAsInteger());
        } else if (isRelationalOperator) {
            termTree->right = getAsIs();
        } else {
            termTree->right = std::make_shared<DoubleTermTree>(getAsDouble());
        }
    }

    availableValue = AvailableValue::NONE;
    if (leftFinal && rightFinal) {
        if (isStringConcat) {
            std::string left = std::static_pointer_cast<StringLiteralTermTree>(termTree->left)->value;
            std::string right = std::static_pointer_cast<StringLiteralTermTree>(termTree->right)->value;
            stringValue = left + right;
            availableValue = AvailableValue::STRING;
        } else if (isBitwiseOperator) {
            int64_t left = std::static_pointer_cast<IntegerTermTree>(termTree->left)->value;
            int64_t right = std::static_pointer_cast<IntegerTermTree>(termTree->right)->value;
            integerValue = calculateBitwise(left, right, termTree->binaryOperator);
            availableValue = AvailableValue::INTEGER;
        } else if (isRelationalOperator) {
            try {
                booleanValue = calculateRelational(termTree->left, termTree->right, termTree->binaryOperator);
                availableValue = AvailableValue::BOOLEAN;
            } catch (std::exception &exception) {
                availableValue = AvailableValue::NONE;
                return;
            }
        } else {
            long double left = std::static_pointer_cast<DoubleTermTree>(termTree->left)->value;
            long double right = std::static_pointer_cast<DoubleTermTree>(termTree->right)->value;
            doubleValue = calculateArithmetic(left, right, termTree->binaryOperator);
            availableValue = AvailableValue::DOUBLE;
        }
    }
}

void ConstantExpressionEvaluationVisitor::visitLogicalExpressionTermTree(LogicalExpressionTermTree *termTree) {
    bool left = false;
    bool leftTerminal = false;
    if (termTree->left) {
        availableValue = AvailableValue::NONE;
        termTree->left->accept(this);
        if (isAvailable()) {
            termTree->left = getAsIs();
            left = getAsBoolean();
            leftTerminal = true;
        }
    }

    bool right = false;
    bool rightTerminal = false;
    if (termTree->right) {
        availableValue = AvailableValue::NONE;
        termTree->right->accept(this);
        if (isAvailable()) {
            termTree->right = getAsIs();
            right = getAsBoolean();
            rightTerminal = true;
        }
    }

    if (leftTerminal && rightTerminal) {
        availableValue = AvailableValue::BOOLEAN;
        switch (termTree->binaryOperator) {
            case Keyword::AND:
                booleanValue = left && right;
                break;
            case Keyword::OR:
                booleanValue = left || right;
                break;
            default:
                break;
        }
    } else {
        availableValue = AvailableValue::NONE;
    }
}

void ConstantExpressionEvaluationVisitor::visitTableFieldTermTree(TableFieldTermTree *termTree) {

    if (termTree->left) {
        availableValue = AvailableValue::NONE;
        termTree->left->accept(this);
        if (isAvailable()) {
            termTree->left = getAsIs();
        }
    }

    if (termTree->right) {
        availableValue = AvailableValue::NONE;
        termTree->right->accept(this);
        if (isAvailable()) {
            termTree->right = getAsIs();
        }
    }

    availableValue = AvailableValue::NONE;
}

void ConstantExpressionEvaluationVisitor::visitTableConstructorTermTree(TableConstructorTermTree *termTree) {
    for (auto &field: termTree->fields) {
        field->accept(this);
    }
}

void ConstantExpressionEvaluationVisitor::visitExpressionListTree(ExpressionListTree *listTree) {
    for (auto &expr: listTree->expressions) {
        availableValue = AvailableValue::NONE;
        expr->accept(this);
        if (isAvailable()) {
            expr = getAsIs();
        }
    }
    availableValue = AvailableValue::NONE;
}


void ConstantExpressionEvaluationVisitor::visitFunctionCallSuffixTree(FunctionCallSuffixTree *suffixTree) {
    suffixTree->arguments->accept(this);
}

void ConstantExpressionEvaluationVisitor::visitTableFieldAccessSuffixTree(TableFieldAccessSuffixTree *suffixTree) {

    if (suffixTree->expressionTree) {
        availableValue = AvailableValue::NONE;
        suffixTree->expressionTree->accept(this);
        if (isAvailable()) {
            suffixTree->expressionTree = getAsIs();
        }
    }

    availableValue = AvailableValue::NONE;
}


void ConstantExpressionEvaluationVisitor::visitVariableTree(VariableTree *variableTree) {
    if (variableTree->expression) {
        availableValue = AvailableValue::NONE;
        variableTree->expression->accept(this);
        if (isAvailable()) {
            variableTree->expression = getAsIs();
        }
    }

    for (auto &suffix: variableTree->prefixExpressionSuffix) {
        availableValue = AvailableValue::NONE;
        suffix->accept(this);
        if (isAvailable()) {
            suffix = getAsIs();
        }
    }

    availableValue = AvailableValue::NONE;

}

void ConstantExpressionEvaluationVisitor::visitVariableListTree(VariableListTree *listTree) {
    for (auto &var: listTree->variableTrees) {
        var->accept(this);
    }
}


void ConstantExpressionEvaluationVisitor::visitFunctionBodyTree(FunctionBodyTree *bodyTree) {
    if (bodyTree->body) {
        availableValue = AvailableValue::NONE;
        bodyTree->body->accept(this);
    }
}

void ConstantExpressionEvaluationVisitor::visitAssignmentTree(AssignmentTree *assignmentTree) {
    assignmentTree->variableList->accept(this);
    assignmentTree->expressionList->accept(this);
}

void ConstantExpressionEvaluationVisitor::visitAttributeIdentifierAssignmentTree(
        AttributeIdentifierAssignmentTree *assignmentTree
) {
    if (assignmentTree->expressionListTree) {
        availableValue = AvailableValue::NONE;
        assignmentTree->expressionListTree->accept(this);
    }
}

void ConstantExpressionEvaluationVisitor::visitReturnStatementTree(ReturnStatementTree *statementTree) {
    if (statementTree->expressions) {
        statementTree->expressions->accept(this);
    }
}

void ConstantExpressionEvaluationVisitor::visitForLoopStatementTree(ForLoopStatementTree *statementTree) {
    if (statementTree->expressions) {
        availableValue = AvailableValue::NONE;
        statementTree->expressions->accept(this);
    }

    if (statementTree->body) {
        availableValue = AvailableValue::NONE;
        statementTree->body->accept(this);
    }
}

void ConstantExpressionEvaluationVisitor::visitFunctionDefinitionTree(FunctionDefinitionTree *definitionTree) {
    if (definitionTree->functionBody) {
        availableValue = AvailableValue::NONE;
        definitionTree->functionBody->accept(this);
    }
}

void ConstantExpressionEvaluationVisitor::visitWhileLoopTree(WhileLoopTree *loopTree) {
    if (loopTree->expression) {
        loopTree->expression->accept(this);
    }

    if (loopTree->body) {
        loopTree->body->accept(this);
    }
}

void ConstantExpressionEvaluationVisitor::visitRepeatUntilTree(RepeatUntilTree *untilTree) {
    if (untilTree->expression) {
        untilTree->expression->accept(this);
    }

    if (untilTree->body) {
        untilTree->body->accept(this);
    }
}

void ConstantExpressionEvaluationVisitor::visitIfThenElseStatementTree(IfThenElseStatementTree *statementTree) {
    for (auto &[exp, body]: statementTree->ifExpThenBlocks) {
        if (exp) {
            availableValue = AvailableValue::NONE;
            exp->accept(this);
            exp = getAsIs();
        }

        if (body) {
            availableValue = AvailableValue::NONE;
            body->accept(this);
        }
    }
}

void ConstantExpressionEvaluationVisitor::visitProgramTree(ProgramTree *programTree) {
    for (const auto &statement: programTree->statements) {
        statement->accept(this);
    }
}

bool ConstantExpressionEvaluationVisitor::isAvailable() {
    return availableValue != AvailableValue::NONE;
}

void ConstantExpressionEvaluationVisitor::evalAllExpressions(aux::ir::tree::AbstractSyntaxTree *tree) {
    tree->accept(this);
}

long double ConstantExpressionEvaluationVisitor::getAsDouble() const {
    // todo: add readable exceptions
    switch (availableValue) {
        case AvailableValue::STRING:
            return std::stold(stringValue);
        case AvailableValue::INTEGER:
            return static_cast<long double>(integerValue);
        case AvailableValue::DOUBLE:
            return doubleValue;
        case AvailableValue::BOOLEAN:
            return booleanValue ? 1.0 : 0.0;
        case AvailableValue::NONE:
            throw std::invalid_argument("ConstantExpressionEvaluation::getAsDouble called when value is unavailable");
    }

    return 0.0;
}

std::string ConstantExpressionEvaluationVisitor::getAsString() const {
    // todo: add readable exceptions
    switch (availableValue) {
        case AvailableValue::STRING:
            return stringValue;
        case AvailableValue::INTEGER:
            return std::to_string(integerValue);
        case AvailableValue::DOUBLE:
            return std::to_string(doubleValue);
        case AvailableValue::BOOLEAN:
            return booleanValue ? "True" : "False";
        case AvailableValue::NONE:
            throw std::invalid_argument("ConstantExpressionEvaluation::getAsString called when value is unavailable");
    }

    return "";
}

int64_t ConstantExpressionEvaluationVisitor::getAsInteger() const {
    // todo: add readable exceptions
    switch (availableValue) {
        case AvailableValue::STRING:
            return std::stoll(stringValue);
        case AvailableValue::INTEGER:
            return integerValue;
        case AvailableValue::DOUBLE:
            return static_cast<int64_t>(doubleValue);
        case AvailableValue::BOOLEAN:
            return booleanValue ? 1 : 0;
        case AvailableValue::NONE:
            throw std::invalid_argument("ConstantExpressionEvaluation::getAsInteger called when value is unavailable");
    }

    return 0;
}

bool ConstantExpressionEvaluationVisitor::getAsBoolean() const {
    switch (availableValue) {
        case AvailableValue::STRING:
            return !stringValue.empty();
        case AvailableValue::INTEGER:
            return integerValue != 0;
        case AvailableValue::DOUBLE:
            return doubleValue != 0;
        case AvailableValue::BOOLEAN:
            return booleanValue;
        case AvailableValue::NONE:
            throw std::invalid_argument("ConstantExpressionEvaluation::getAsBoolean called when value is unavailable");
    }
    return false;
}

std::shared_ptr<aux::ir::tree::TermTree> ConstantExpressionEvaluationVisitor::getAsIs() const {
    switch (availableValue) {
        case AvailableValue::STRING:
            return std::make_shared<StringLiteralTermTree>(getAsString());
        case AvailableValue::INTEGER:
            return std::make_shared<IntegerTermTree>(getAsInteger());
        case AvailableValue::DOUBLE:
            return std::make_shared<DoubleTermTree>(getAsDouble());
        case AvailableValue::BOOLEAN:
            return std::make_shared<KeywordTermTree>(getAsBoolean() ? Keyword::TRUE : Keyword::FALSE);
        case AvailableValue::NONE:
            throw std::invalid_argument("ConstantExpressionEvaluation::getAsIs called when value is unavailable");
    }
    return nullptr;
}

long double
ConstantExpressionEvaluationVisitor::calculateArithmetic(long double left, long double right, Operator op) const {
    switch (op) {
        case Operator::PLUS:
            return left + right;
        case Operator::MINUS:
            return left - right;
        case Operator::ASTERISK:
            return left * right;
        case Operator::SLASH:
            return left / right;
        case Operator::PERCENT:
            return int64_t(left) % int64_t(right);
        case Operator::SLASH_SLASH:
            return floorl(left) / floorl(right);
        default:
            throw std::invalid_argument(
                    "ConstantExpressionEvaluation::calculateArithmetic called with operator " + *op
            );
    }
    return 0;
}

int64_t ConstantExpressionEvaluationVisitor::calculateBitwise(int64_t left, int64_t right, Operator op) const {
    switch (op) {
        case Operator::AMPERSAND:
            return left & right;
        case Operator::TILDA:
            return left ^ right;
        case Operator::VERTICAL_BAR:
            return left | right;
        case Operator::LT_LT:
            return left << right;
        case Operator::GT_GT:
            return left >> right;
        default:
            throw std::invalid_argument(
                    "ConstantExpressionEvaluation::calculateBitwise called with operator " + *op
            );
    }
    return 0;
}

bool ConstantExpressionEvaluationVisitor::calculateRelational(
        const std::shared_ptr<aux::ir::tree::TermTree> &left,
        const std::shared_ptr<aux::ir::tree::TermTree> &right,
        aux::ir::tokens::Operator op
) const {
    switch (op) {
        case Operator::TILDA_EQUAL:
            return !calculateRelational(left, right, tokens::Operator::EQUAL_EQUAL);
        case Operator::EQUAL_EQUAL: {
            if (auto leftStr = std::dynamic_pointer_cast<StringLiteralTermTree>(left)) {
                if (auto rightStr = std::dynamic_pointer_cast<StringLiteralTermTree>(right)) {
                    return leftStr->value == rightStr->value;
                } else {
                    return false;
                }
            } else if (auto leftInt = std::dynamic_pointer_cast<IntegerTermTree>(left)) {
                if (auto rightInt = std::dynamic_pointer_cast<IntegerTermTree>(right)) {
                    return leftInt->value == rightInt->value;
                } else if (auto rightDouble = std::dynamic_pointer_cast<DoubleTermTree>(right)) {
                    return static_cast<long double>(leftInt->value) == rightDouble->value;
                } else {
                    return false;
                }
            } else if (auto leftDouble = std::dynamic_pointer_cast<DoubleTermTree>(left)) {
                if (auto rightInt = std::dynamic_pointer_cast<IntegerTermTree>(right)) {
                    return leftDouble->value == static_cast<long double>(rightInt->value);
                } else if (auto rightDouble = std::dynamic_pointer_cast<DoubleTermTree>(right)) {
                    return leftDouble->value == rightDouble->value;
                } else {
                    return false;
                }
            } else if (auto leftKeyword = std::dynamic_pointer_cast<KeywordTermTree>(left)) {
                if (auto rightKeyword = std::dynamic_pointer_cast<KeywordTermTree>(right)) {
                    return leftKeyword->keyword == rightKeyword->keyword;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        case Operator::LESS_THAN:
            return !calculateRelational(left, right, Operator::GT_EQUAL);
        case Operator::GREATER_THAN:
            return !calculateRelational(left, right, Operator::LT_EQUAL);
        case Operator::LT_EQUAL:
        case Operator::GT_EQUAL: {
            if (auto leftInt = std::dynamic_pointer_cast<IntegerTermTree>(left)) {
                if (auto rightInt = std::dynamic_pointer_cast<IntegerTermTree>(right)) {
                    if (op == tokens::Operator::LT_EQUAL) {
                        return leftInt->value <= rightInt->value;
                    } else {
                        return leftInt->value >= rightInt->value;
                    }
                } else if (auto rightDouble = std::dynamic_pointer_cast<DoubleTermTree>(right)) {
                    if (op == tokens::Operator::LT_EQUAL) {
                        return static_cast<long double>(leftInt->value) <= rightDouble->value;
                    } else {
                        return static_cast<long double>(leftInt->value) >= rightDouble->value;
                    }
                } else {
                    throw std::invalid_argument(
                            "ConstantExpressionEvaluation::calculateRelational called with operands (numeric: non-numeric)"
                    );
                }
            } else if (auto leftDouble = std::dynamic_pointer_cast<DoubleTermTree>(left)) {
                if (auto rightInt = std::dynamic_pointer_cast<IntegerTermTree>(right)) {
                    if (op == tokens::Operator::LT_EQUAL) {
                        return leftDouble->value <= static_cast<long double>(rightInt->value);
                    } else {
                        return leftDouble->value >= static_cast<long double>(rightInt->value);
                    }
                } else if (auto rightDouble = std::dynamic_pointer_cast<DoubleTermTree>(right)) {
                    if (op == tokens::Operator::LT_EQUAL) {
                        return leftDouble->value <= rightDouble->value;
                    } else {
                        return leftDouble->value >= rightDouble->value;
                    }
                } else {
                    throw std::invalid_argument(
                            "ConstantExpressionEvaluation::calculateRelational called with operands (numeric: non-numeric)"
                    );
                }
            } else if (auto leftStr = std::dynamic_pointer_cast<StringLiteralTermTree>(left)) {
                if (auto rightStr = std::dynamic_pointer_cast<StringLiteralTermTree>(right)) {
                    if (op == tokens::Operator::LT_EQUAL) {
                        return leftStr->value <= rightStr->value;
                    } else {
                        return leftStr->value >= rightStr->value;
                    }
                } else {
                    throw std::invalid_argument(
                            "ConstantExpressionEvaluation::calculateRelational called with operands (string: non-string)"
                    );
                }
            } else {
                throw std::invalid_argument(
                        "ConstantExpressionEvaluation::calculateRelational called with invalid operand types"
                );
            }
        }
        default:
            throw std::invalid_argument(
                    "ConstantExpressionEvaluation::calculateRelational called with operator " + *op
            );
    }
    return false;
}

