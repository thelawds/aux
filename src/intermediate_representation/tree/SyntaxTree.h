//
// Created by miserable on 03.03.2022.
//

#ifndef AUX_SYNTAXTREE_H
#define AUX_SYNTAXTREE_H

#include <memory>
#include <utility>
#include <vector>
#include "intermediate_representation/Token.h"
#include "semantics/AstVisitor.h"
#include "util/Defines.h"

namespace aux::ir::syntax_tree {

    using namespace tokens;

    /**
     * <b> Base Trees: </b>
     */

    struct AbstractSyntaxTree {

        virtual void accept(aux::semantics::AstVisitor *visitor) = 0;

        virtual ~AbstractSyntaxTree() = default;

    };

    MARKER_INTERFACE
    struct StatementTree : virtual AbstractSyntaxTree {};

    MARKER_INTERFACE
    struct PrefixExpressionSuffixTree : virtual AbstractSyntaxTree {};

    MARKER_INTERFACE
    struct ExpressionTree : virtual AbstractSyntaxTree, virtual PrefixExpressionSuffixTree {};

    MARKER_INTERFACE
    struct TermTree : virtual AbstractSyntaxTree, virtual ExpressionTree, virtual StatementTree {};

    MARKER_INTERFACE
    struct BinaryExpressionTree : virtual AbstractSyntaxTree, virtual ExpressionTree {};

    MARKER_INTERFACE
    struct UnaryExpressionTree : virtual AbstractSyntaxTree, virtual ExpressionTree {};

    MARKER_INTERFACE
    struct ArgumentsTree : virtual AbstractSyntaxTree {};

    struct ProgramTree : StatementTree {
        std::vector<std::shared_ptr<StatementTree>> statements;

        inline void addStatement(const std::shared_ptr<StatementTree> &statement) {
            statements.push_back(statement);
        }

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitProgramTree(this);
        }
    };

    /**
     * <b> Concrete Trees: </b>
     */

    struct EmptyStatementTree : StatementTree {
        inline void accept(aux::semantics::AstVisitor *visitor) override {
            throw std::logic_error("Empty Statement should not be visited");
        }
    };

    struct KeywordTermTree : TermTree {
        const Keyword keyword;
        const Span span;

        inline explicit KeywordTermTree(const Keyword keyword) : keyword(keyword), span({0, 0}) {}

        inline explicit KeywordTermTree(
                const std::shared_ptr<TokenKeyword> &tokenKeyword
        ) : keyword(tokenKeyword->getKeyword()),
            span(tokenKeyword->getSpan()) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitKeywordTerm(this);
        }
    };

    struct KeywordStatementTree : StatementTree {
        const Keyword keyword;
        const Span span;

        inline explicit KeywordStatementTree(
                const std::shared_ptr<TokenKeyword> &tokenKeyword
        ) : keyword(tokenKeyword->getKeyword()),
            span(tokenKeyword->getSpan()) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitKeywordStatementTree(this);
        }
    };

    struct GotoStatementTree : StatementTree {
        std::shared_ptr<TokenIdentifier> identifier;

        inline explicit GotoStatementTree(std::shared_ptr<TokenIdentifier> identifier)
                : identifier(std::move(identifier)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitGotoStatementTree(this);
        }
    };

    struct IdentifierTermTree : TermTree {
        const std::string value;
        const Span span;

        inline explicit IdentifierTermTree(
                const std::shared_ptr<TokenIdentifier> &tokenIdentifier
        ) : value(tokenIdentifier->getValue()),
            span(tokenIdentifier->getSpan()) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitIdentifierTerm(this);
        }

    };

    struct IntegerTermTree : TermTree {
        const uint64_t value;
        const Span span;

        inline explicit IntegerTermTree(const uint64_t value) : value(value), span({0, 0}) {}

        inline explicit IntegerTermTree(
                const std::shared_ptr<TokenDecimal> &tokenNumeric
        ) : value(tokenNumeric->getValue()),
            span(tokenNumeric->getSpan()) {}

        inline explicit IntegerTermTree(
                const std::shared_ptr<TokenHex> &tokenNumeric
        ) : value(tokenNumeric->getValue()),
            span(tokenNumeric->getSpan()) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitIntegerTerm(this);
        }

    };

    struct DoubleTermTree : TermTree {
        const long double value;
        const Span span;

        inline explicit DoubleTermTree(
                const std::shared_ptr<TokenDouble> &tokenNumeric
        ) : value(tokenNumeric->getValue()),
            span(tokenNumeric->getSpan()) {}

        inline explicit DoubleTermTree(long double value) : value(value), span({0, 0}) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitDoubleTerm(this);
        }

    };

    struct StringLiteralTermTree : TermTree, ArgumentsTree {
        const std::string value;
        const Span span;

        inline explicit StringLiteralTermTree(std::string value)
                : value(std::move(value)), span({0, 0}) {}

        inline explicit StringLiteralTermTree(
                const std::shared_ptr<TokenStringLiteral> &tokenStringLiteral
        ) : value(tokenStringLiteral->getValue()),
            span(tokenStringLiteral->getSpan()) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitStringLiteralTerm(this);
        }

    };

    struct PrefixExpressionTermTree : TermTree {
        std::shared_ptr<ExpressionTree> expressionOrIdentifier;
        std::vector<std::shared_ptr<PrefixExpressionSuffixTree>> peSuffixTrees;

        inline explicit PrefixExpressionTermTree(std::shared_ptr<ExpressionTree> expressionOrIdentifier)
                : expressionOrIdentifier(std::move(expressionOrIdentifier)) {}

        inline void addPrefixExpressionSuffix(const std::shared_ptr<PrefixExpressionSuffixTree> &suffixTree) {
            peSuffixTrees.push_back(suffixTree);
        }

        void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitPrefixExpressionTermTree(this);
        }

    };

    struct ExponentTermTree : BinaryExpressionTree, TermTree {
        const std::shared_ptr<TermTree> left;
        const std::shared_ptr<TermTree> right;
        const Operator binaryOperator = Operator::CARET;

        inline ExponentTermTree(
                std::shared_ptr<TermTree> left, std::shared_ptr<TermTree> right
        ) : left(std::move(left)),
            right(std::move(right)) {}

        inline explicit ExponentTermTree(std::shared_ptr<TermTree> left) : left(std::move(left)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitExponentTermTree(this);
        }

    };

    struct UnaryTermTree : UnaryExpressionTree, TermTree {
        union UnaryTermOperator {
            Keyword keyword;
            Operator unaryOperator;
        };

        const bool isKeywordOperator;
        const UnaryTermOperator unaryTermOperator;
        std::shared_ptr<TermTree> exponentTermTree;

        inline UnaryTermTree(
                const std::shared_ptr<TokenKeyword> &keyword, std::shared_ptr<TermTree> exponentTermTree
        ) : isKeywordOperator(true),
            unaryTermOperator({.keyword = keyword->getKeyword()}),
            exponentTermTree(std::move(exponentTermTree)) {}

        inline UnaryTermTree(
                const std::shared_ptr<TokenOperator> &op, std::shared_ptr<TermTree> exponentTermTree
        ) : isKeywordOperator(false),
            unaryTermOperator({.unaryOperator = op->getOperator()}),
            exponentTermTree(std::move(exponentTermTree)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitUnaryTermTree(this);
        }

    };

    struct BinaryExpressionTermTree : BinaryExpressionTree, TermTree {
        std::shared_ptr<TermTree> left;
        std::shared_ptr<TermTree> right;
        const Operator binaryOperator;
        const Span operatorSpan;

        inline BinaryExpressionTermTree(
                std::shared_ptr<TermTree> left,
                std::shared_ptr<TermTree> right,
                const std::shared_ptr<TokenOperator> &tokenOperator
        ) : left(std::move(left)),
            right(std::move(right)),
            binaryOperator(tokenOperator->getOperator()),
            operatorSpan(tokenOperator->getSpan()) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitBinaryExpressionTermTree(this);
        }

    };


    struct LogicalExpressionTermTree : BinaryExpressionTree, TermTree {
        std::shared_ptr<TermTree> left;
        std::shared_ptr<TermTree> right;
        const Keyword binaryOperator;
        const Span operatorSpan;

        inline LogicalExpressionTermTree(
                std::shared_ptr<TermTree> left,
                std::shared_ptr<TermTree> right,
                const std::shared_ptr<TokenKeyword> &tokenKeyword
        ) : left(std::move(left)),
            right(std::move(right)),
            binaryOperator(tokenKeyword->getKeyword()),
            operatorSpan(tokenKeyword->getSpan()) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitLogicalExpressionTermTree(this);
        }

    };

    struct TableFieldTermTree : BinaryExpressionTree, TermTree {
        std::shared_ptr<ExpressionTree> left;
        std::shared_ptr<ExpressionTree> right;
        const Operator binaryOperator = Operator::EQUAL;

        inline TableFieldTermTree(
                std::shared_ptr<ExpressionTree> left, std::shared_ptr<ExpressionTree> right
        ) : left(std::move(left)),
            right(std::move(right)) {}

        inline explicit TableFieldTermTree(std::shared_ptr<ExpressionTree> right) : right(std::move(right)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitTableFieldTermTree(this);
        }

    };

    struct TableConstructorTermTree : TermTree, ArgumentsTree {
        std::vector<std::shared_ptr<TableFieldTermTree>> fields;

        inline void addField(const std::shared_ptr<TableFieldTermTree> &field) {
            fields.push_back(field);
        }

        virtual void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitTableConstructorTermTree(this);
        }

    };

    struct ExpressionListTree : ArgumentsTree {
        std::vector<std::shared_ptr<ExpressionTree>> expressions;

        inline void addExpression(const std::shared_ptr<ExpressionTree> &expression) {
            if (expression) {
                expressions.push_back(expression);
            }
        }

        void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitExpressionListTree(this);
        }

    };

    struct FunctionCallSuffixTree : PrefixExpressionSuffixTree {
        std::shared_ptr<TokenIdentifier> identifier;
        std::shared_ptr<ArgumentsTree> arguments;

        inline FunctionCallSuffixTree(
                std::shared_ptr<TokenIdentifier> identifier,
                std::shared_ptr<ArgumentsTree> arguments
        ) : identifier(std::move(identifier)),
            arguments(std::move(arguments)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitFunctionCallSuffixTree(this);
        }

    };

    struct TableFieldAccessSuffixTree : PrefixExpressionSuffixTree {
        std::shared_ptr<ExpressionTree> expressionTree;

        inline explicit TableFieldAccessSuffixTree(std::shared_ptr<ExpressionTree> expressionTree)
                : expressionTree(std::move(expressionTree)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitTableFieldAccessSuffixTree(this);
        }
    };

    struct StructAccessSuffixTree : PrefixExpressionSuffixTree {
        std::shared_ptr<TokenIdentifier> identifier; // .Identifier

        inline explicit StructAccessSuffixTree(std::shared_ptr<TokenIdentifier> identifier)
                : identifier(std::move(identifier)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitStructAccessSuffixTree(this);
        }
    };

    struct VariableTree : AbstractSyntaxTree {
        std::shared_ptr<IdentifierTermTree> identifier;
        std::vector<std::shared_ptr<PrefixExpressionSuffixTree>> prefixExpressionSuffix;

        inline explicit VariableTree(std::shared_ptr<IdentifierTermTree> identifier)
                : identifier(std::move(identifier)) {}

        VariableTree(
                std::shared_ptr<IdentifierTermTree> identifier,
                std::vector<std::shared_ptr<PrefixExpressionSuffixTree>> prefixExpressionSuffix
        ) : identifier(std::move(identifier)), prefixExpressionSuffix(std::move(prefixExpressionSuffix)) {}

        inline void addPrefixExprSuffix(const std::shared_ptr<PrefixExpressionSuffixTree> &suffix) {
            prefixExpressionSuffix.push_back(suffix);
        }

        void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitVariableTree(this);
        }

    };

    struct VariableListTree : AbstractSyntaxTree {
        std::vector<std::shared_ptr<VariableTree>> variableTrees;

        inline void addVariable(const std::shared_ptr<VariableTree> &variableTree) {
            variableTrees.push_back(variableTree);
        }

        void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitVariableListTree(this);
        }

    };

    struct AttributeTree : IdentifierTermTree { // todo: maybe remove unnecessary inheritance?

        using IdentifierTermTree::IdentifierTermTree;

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitAttributeTree(this);
        }
    };

    struct IdentifierTermListTree : ExpressionListTree {
        std::vector<std::shared_ptr<TermTree>> identifiers{};

        inline IdentifierTermListTree() = default;

        inline void addIdentifier(const std::shared_ptr<IdentifierTermTree> &identifierTermTree) {
            if (identifierTermTree) {
                identifiers.push_back(identifierTermTree);
            }
        }

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitIdentifierTermListTree(this);
        }

    };

    struct ParameterListTree : AbstractSyntaxTree {
        std::shared_ptr<IdentifierTermListTree> identifierTermListTree;
        bool hasTrailing3DotsOperator = false;

        inline ParameterListTree(
                std::shared_ptr<IdentifierTermListTree> identifierTermListTree,
                bool hasTrailing3DotsOperator
        ) : identifierTermListTree(std::move(identifierTermListTree)),
            hasTrailing3DotsOperator(hasTrailing3DotsOperator) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitParameterListTree(this);
        }

    };

    struct AttributeIdentifierListTree : StatementTree {
        using AttributeIdentifierPair = std::pair<std::shared_ptr<IdentifierTermTree>, std::shared_ptr<AttributeTree>>;

        std::vector<AttributeIdentifierPair> attributeIdentifiers;

        inline void addAttributeIdentifier(
                const std::shared_ptr<IdentifierTermTree> &identifierTermTree,
                const std::shared_ptr<AttributeTree> &attributeTree
        ) {
            attributeIdentifiers.emplace_back(identifierTermTree, attributeTree);
        }

        virtual void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitAttributeIdentifierListTree(this);
        }

    };

    struct LabelTree : StatementTree {
        std::shared_ptr<TokenIdentifier> identifier;

        explicit inline LabelTree(std::shared_ptr<TokenIdentifier> identifier) : identifier(std::move(identifier)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitLabelTree(this);
        }
    };

    struct FunctionBodyTree : ExpressionTree {
        std::vector<std::shared_ptr<IdentifierTermTree>> parameters;
        bool hasTrailingDots{false};
        std::shared_ptr<ProgramTree> body;

        inline explicit FunctionBodyTree(std::shared_ptr<ProgramTree> body) : body(std::move(body)) {}

        inline FunctionBodyTree(const std::shared_ptr<ParameterListTree> &params, std::shared_ptr<ProgramTree> body)
                : body(std::move(body)), hasTrailingDots(params->hasTrailing3DotsOperator) {

            for (auto &el: params->identifierTermListTree->identifiers) {
                parameters.push_back(std::static_pointer_cast<IdentifierTermTree>(el));
            }
        }

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitFunctionBodyTree(this);
        }
    };

    struct FunctionIdentifierTree : AbstractSyntaxTree {
        std::shared_ptr<TokenIdentifier> primaryIdentifier;
        std::vector<std::shared_ptr<TokenIdentifier>> secondaryIdentifiers;
        std::shared_ptr<TokenIdentifier> colonIdentifier;

        inline explicit FunctionIdentifierTree(std::shared_ptr<TokenIdentifier> primaryIdentifier)
                : primaryIdentifier(std::move(primaryIdentifier)) {}

        inline void addSecondaryIdentifier(const std::shared_ptr<TokenIdentifier> &secondaryIdentifier) {
            secondaryIdentifiers.push_back(secondaryIdentifier);
        }

        inline void setColonIdentifier(const std::shared_ptr<TokenIdentifier> &identifier) {
            colonIdentifier = identifier;
        }

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitFunctionIdentifierTree(this);
        }

    };

    struct AssignmentTree : StatementTree {
        std::shared_ptr<VariableListTree> variableList;
        std::shared_ptr<ExpressionListTree> expressionList;

        inline AssignmentTree(
                std::shared_ptr<VariableListTree> variableList,
                std::shared_ptr<ExpressionListTree> expressionList
        ) : variableList(std::move(variableList)), expressionList(std::move(expressionList)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitAssignmentTree(this);
        }
    };

    struct AttributeIdentifierAssignmentTree : StatementTree {
        std::shared_ptr<AttributeIdentifierListTree> attributeIdentifierListTree;
        std::shared_ptr<ExpressionListTree> expressionListTree;

        inline AttributeIdentifierAssignmentTree(
                std::shared_ptr<AttributeIdentifierListTree> attributeIdentifierListTree,
                std::shared_ptr<ExpressionListTree> expressionListTree
        ) : attributeIdentifierListTree(std::move(attributeIdentifierListTree)),
            expressionListTree(std::move(expressionListTree)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitAttributeIdentifierAssignmentTree(this);
        }

    };

    struct ReturnStatementTree : StatementTree {
        std::shared_ptr<ExpressionListTree> expressions;

        explicit inline ReturnStatementTree(std::shared_ptr<ExpressionListTree> expressions)
                : expressions(std::move(expressions)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitReturnStatementTree(this);
        }
    };

    struct ForLoopStatementTree : StatementTree {

        std::shared_ptr<IdentifierTermListTree> identifiers;
        std::shared_ptr<ExpressionListTree> expressions;
        std::shared_ptr<ProgramTree> body;

        inline ForLoopStatementTree(
                std::shared_ptr<IdentifierTermListTree> identifierList,
                std::shared_ptr<ExpressionListTree> expressionList,
                std::shared_ptr<ProgramTree> body
        ) : identifiers(std::move(identifierList)), expressions(std::move(expressionList)), body(std::move(body)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitForLoopStatementTree(this);
        }

    };

    struct FunctionDefinitionTree : StatementTree, ExpressionTree {
        std::shared_ptr<FunctionIdentifierTree> identifier;
        std::shared_ptr<FunctionBodyTree> functionBody;
        bool isLocal{false};

        inline FunctionDefinitionTree(
                std::shared_ptr<FunctionIdentifierTree> identifier,
                std::shared_ptr<FunctionBodyTree> functionBody
        ) : identifier(std::move(identifier)), functionBody(std::move(functionBody)) {}

        inline explicit FunctionDefinitionTree(std::shared_ptr<FunctionBodyTree> functionBody)
                : functionBody(std::move(functionBody)), identifier(nullptr) {}

        inline void makeLocal() {
            isLocal = true;
        }

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitFunctionDefinitionTree(this);
        }

    };

    struct WhileLoopTree : StatementTree {
        std::shared_ptr<ExpressionTree> expression;
        std::shared_ptr<ProgramTree> body;

        inline WhileLoopTree(std::shared_ptr<ExpressionTree> expression, std::shared_ptr<ProgramTree> body)
                : expression(std::move(expression)), body(std::move(body)) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitWhileLoopTree(this);
        }
    };

    struct RepeatUntilTree : WhileLoopTree {

        inline RepeatUntilTree(
                const std::shared_ptr<ExpressionTree> &expression,
                const std::shared_ptr<ProgramTree> &body
        ) : WhileLoopTree(expression, body) {}

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitRepeatUntilTree(this);
        }

    };

    struct IfThenElseStatementTree : StatementTree {
        using ExpThenBlock = std::pair<std::shared_ptr<ExpressionTree>, std::shared_ptr<ProgramTree>>;

        std::vector<ExpThenBlock> ifExpThenBlocks;

        inline void addExpressionAndBody(
                const std::shared_ptr<ExpressionTree> &expression,
                const std::shared_ptr<ProgramTree> &body
        ) {
            ifExpThenBlocks.emplace_back(expression, body);
        }

        inline void addElseBody(const std::shared_ptr<ProgramTree> &body) {
            ifExpThenBlocks.emplace_back(nullptr, body);
        }

        inline void accept(aux::semantics::AstVisitor *visitor) override {
            visitor->visitIfThenElseStatementTree(this);
        }

    };


}


#endif //AUX_SYNTAXTREE_H
