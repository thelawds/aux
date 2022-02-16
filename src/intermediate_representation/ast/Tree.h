//
// Created by miserable on 10.02.2022.
//

#ifndef AUX_TREE_H
#define AUX_TREE_H

#include <memory>
#include <utility>
#include "../tokens/Token.h"

namespace aux::ir::ast {

    struct BaseTree {

        virtual std::string getPrintValue() = 0;

        virtual std::shared_ptr<BaseTree> getLeft() = 0;

        virtual std::shared_ptr<BaseTree> getRight() = 0;

    };

    struct BinTree : BaseTree {
        enum class Type {
            NONE,
            TABLE_FIELD_DECLARATION,
            BINARY_OPERATION,
            UNARY_OPERATION,
            ATTRIBUTE_IDENTIFIER,
            LIST_ELEM,
            FUNCTION_BODY,
            FUNCTION_DEFINITION,
            LOCAL_FUNCTION_DEFINITION,
            WHILE_LOOP,
            REPEAT_UNTIL_LOOP,
            IF_THEN,
            ELSE,
            FUNCTION_CALL
        };

        const Type type{Type::NONE};
        std::shared_ptr<BaseTree> left;
        std::shared_ptr<BaseTree> right;
        std::shared_ptr<tokens::Token> op;

        BinTree() = default;

        explicit BinTree(const Type type) : type(type) {}

        BinTree(
                Type type,
                std::shared_ptr<BaseTree> left,
                std::shared_ptr<BaseTree> right,
                std::shared_ptr<tokens::Token> op
        ) : type(type), left(std::move(left)), right(std::move(right)), op(std::move(op)) {}

        inline std::string getPrintValue() override {
            static std::unordered_map<Type, std::string> types = {
                    {Type::NONE,                      "None"},
                    {Type::TABLE_FIELD_DECLARATION,   "Table Field Declaration"},
                    {Type::BINARY_OPERATION,          "Binary Operation"},
                    {Type::UNARY_OPERATION,           "Unary Operation"},
                    {Type::ATTRIBUTE_IDENTIFIER,      "Attribute Identifier"},
                    {Type::LIST_ELEM,                 "List Element"},
                    {Type::FUNCTION_BODY,             "Function Body"},
                    {Type::FUNCTION_DEFINITION,       "Function Definition"},
                    {Type::LOCAL_FUNCTION_DEFINITION, "Local Function Definition"},
                    {Type::WHILE_LOOP,                "While Loop"},
                    {Type::REPEAT_UNTIL_LOOP,         "Repeat Until Loop"},
                    {Type::IF_THEN,                   "If/Elseif Branch"},
                    {Type::ELSE,                      "Else"},
                    {Type::FUNCTION_CALL,             "Function Call"}
            };

            std::string result = types[type];

            if (op) {
                result += "\n[  " + op->getRawValue() + "  ]";
            }

            return result;
        }

        inline std::shared_ptr<BaseTree> getLeft() override {
            return left;
        }

        inline std::shared_ptr<BaseTree> getRight() override {
            return right;
        }

    };

    struct ListTree : BaseTree {
        enum class Type {
            NONE,
            TABLE_FIELD_LIST,
            EXPRESSION_LIST,
            PE_SUFFIX_LIST,
            IDENTIFIER_LIST,
            VARIABLE_LIST,
            ATTRIBUTE_IDENTIFIER_LIST,
            FUNCTION_IDENTIFIER_SEQUENCE,
            STATEMENTS_LIST,
            IF_THEN_ELSE
        };

        const Type type{Type::NONE};
        std::vector<std::shared_ptr<BaseTree>> trees;

        ListTree() = default;

        explicit ListTree(Type type) : type(type) {}

        explicit ListTree(std::vector<std::shared_ptr<BaseTree>> trees) : trees(std::move(trees)) {}

        inline void pushBack(const std::shared_ptr<BaseTree> &tree) {
            trees.push_back(tree);
        }

        inline std::string getPrintValue() override {
            static std::unordered_map<Type, std::string> types = {
                    {Type::NONE,                         "None List"},
                    {Type::EXPRESSION_LIST,              "Expression List"},
                    {Type::TABLE_FIELD_LIST,             "Table Field List"},
                    {Type::PE_SUFFIX_LIST,               "Prefix Expression\nSuffix List"},
                    {Type::IDENTIFIER_LIST,              "Identifier List"},
                    {Type::VARIABLE_LIST,                "Variable List"},
                    {Type::ATTRIBUTE_IDENTIFIER_LIST,    "Attributed Identifier List"},
                    {Type::FUNCTION_IDENTIFIER_SEQUENCE, "Function Identifier Sequence"},
                    {Type::STATEMENTS_LIST,              "Statements List"},
                    {Type::IF_THEN_ELSE,                 "If-Else Statement"}
            };

            return types[type];
        }

        inline std::shared_ptr<BaseTree> getLeft() override {
            return {nullptr};
        }

        inline std::shared_ptr<BaseTree> getRight() override {

            if (trees.empty()) {
                return {nullptr};
            }

            auto result = std::make_shared<BinTree>(BinTree::Type::LIST_ELEM);
            std::shared_ptr<BinTree> *curr = &result;
            std::shared_ptr<BinTree> rightTree;

            for (size_t i = 0; i < trees.size(); ++i) {
                (*curr)->left = trees[i];
                if (i != trees.size() - 1) {
                    (*curr)->right = std::make_shared<BinTree>(BinTree::Type::LIST_ELEM);
                    rightTree = std::dynamic_pointer_cast<BinTree>((*curr)->right);
                    curr = &rightTree;
                }
            }

            return result;
        }

    };

    struct TokenTree : BaseTree {
        enum class Type {
            SIMPLE_TOKEN,
            ATTRIBUTE,
            PARAMETER_LIST,
            LABEL,
            DOT_IDENTIFIER,
            COLON_IDENTIFIER,
            GOTO_IDENTIFIER
        };

        const Type type = Type::SIMPLE_TOKEN;
        std::shared_ptr<tokens::Token> token;

        explicit TokenTree(std::shared_ptr<tokens::Token> token) : token(std::move(token)) {}

        TokenTree(const Type type, std::shared_ptr<tokens::Token> token) : type(type), token(std::move(token)) {}

        inline std::string getPrintValue() override {
            std::unordered_map<Type, std::string> printValues{
                    {Type::SIMPLE_TOKEN,     "Token"},
                    {Type::ATTRIBUTE,        "Attribute"},
                    {Type::PARAMETER_LIST,   "ParameterList"},
                    {Type::LABEL,            "Label"},
                    {Type::DOT_IDENTIFIER,   ".Identifier"},
                    {Type::COLON_IDENTIFIER, ":Identifier"},
                    {Type::GOTO_IDENTIFIER,  "Goto"}
            };
            return "(" + printValues[type] + ", " + *token->getType() + ") : " + token->getRawValue();
        }

        inline std::shared_ptr<BaseTree> getLeft() override {
            return nullptr;
        }

        inline std::shared_ptr<BaseTree> getRight() override {
            return nullptr;
        }
    };

    struct ArgsTree : BaseTree {

        std::shared_ptr<tokens::TokenStringLiteral> stringLiteral;
        std::shared_ptr<ListTree> listTree;

        explicit ArgsTree(std::shared_ptr<tokens::TokenStringLiteral> stringLiteral)
                : stringLiteral(std::move(stringLiteral)), listTree(nullptr) {}

        explicit ArgsTree(std::shared_ptr<ListTree> listTree) : listTree(std::move(listTree)) {}

        inline std::string getPrintValue() override {
            return "Function Call Arguments";
        }

        inline std::shared_ptr<BaseTree> getLeft() override {
            if (stringLiteral) {
                return std::make_shared<TokenTree>(stringLiteral);
            } else {
                return nullptr;
            }
        }

        inline std::shared_ptr<BaseTree> getRight() override {
            return listTree;
        }

    };

    struct FunctionCallSuffixTree : BaseTree {
        std::shared_ptr<tokens::TokenIdentifier> identifier;
        std::shared_ptr<ArgsTree> argsTree;

        explicit FunctionCallSuffixTree(
                std::shared_ptr<ArgsTree> argsTree,
                std::shared_ptr<tokens::TokenIdentifier> identifier = nullptr
        ) : argsTree(std::move(argsTree)), identifier(std::move(identifier)) {}

        inline std::string getPrintValue() override {
            return "Function Call Suffix";
        }

        inline std::shared_ptr<BaseTree> getLeft() override {
            if (identifier) {
                return std::make_shared<TokenTree>(identifier);
            }
            return nullptr;
        }

        inline std::shared_ptr<BaseTree> getRight() override {
            return argsTree;
        }

    };

    struct ExprSuffixTree : BaseTree {
        std::shared_ptr<BaseTree> expression; // todo Change type of expression after expression created
        std::shared_ptr<tokens::TokenIdentifier> identifier;

        explicit ExprSuffixTree(std::shared_ptr<BaseTree> expression)
                : expression(std::move(expression)), identifier(nullptr) {}

        explicit ExprSuffixTree(std::shared_ptr<tokens::TokenIdentifier> identifier)
                : identifier(std::move(identifier)), expression(nullptr) {}

        inline std::string getPrintValue() override {
            return "Expression Suffix of " + std::string(expression ? "[Expression]" : ".Identifier");
        }

        inline std::shared_ptr<BaseTree> getLeft() override {
            return expression;
        }

        inline std::shared_ptr<BaseTree> getRight() override {
            if (identifier) {
                return std::make_shared<TokenTree>(identifier);
            }
            return nullptr;
        }

    };

    struct VariableTree : BaseTree {
        std::shared_ptr<tokens::TokenIdentifier> identifier{nullptr};
        std::shared_ptr<BaseTree> expression{nullptr}; // todo change whern expr. is created
        std::shared_ptr<ListTree> exprSuffixes;

        VariableTree(std::shared_ptr<BaseTree> expression, std::shared_ptr<ListTree> exprSuffixes)
                : expression(std::move(expression)), exprSuffixes(std::move(exprSuffixes)) {}

        VariableTree(
                std::shared_ptr<tokens::TokenIdentifier> identifier,
                std::shared_ptr<ListTree> exprSuffixes
        ) : identifier(std::move(identifier)), exprSuffixes(std::move(exprSuffixes)) {}

        inline std::string getPrintValue() override {
            return "Variable Reference";
        }

        inline std::shared_ptr<BaseTree> getLeft() override {
            if (identifier) {
                return std::make_shared<TokenTree>(identifier);
            }
            return expression;
        }

        inline std::shared_ptr<BaseTree> getRight() override {
            return exprSuffixes;
        }

    };

    struct PrefixExprTree : BaseTree {
        std::shared_ptr<tokens::TokenIdentifier> identifier{nullptr};
        std::shared_ptr<BaseTree> expression{nullptr};
        std::shared_ptr<ListTree> suffixes;

        PrefixExprTree(
                std::shared_ptr<tokens::TokenIdentifier> identifier,
                std::shared_ptr<ListTree> suffixes
        ) : identifier(std::move(identifier)), suffixes(std::move(suffixes)) {}

        PrefixExprTree(std::shared_ptr<BaseTree> expression, std::shared_ptr<ListTree> suffixes)
                : expression(std::move(expression)), suffixes(std::move(suffixes)) {}

        inline std::string getPrintValue() override {
            return "Prefix Expression";
        }

        inline std::shared_ptr<BaseTree> getLeft() override {
            if (identifier) {
                return std::make_shared<TokenTree>(identifier);
            }
            return expression;
        }

        inline std::shared_ptr<BaseTree> getRight() override {
            if (suffixes && !suffixes->trees.empty()) {
                return suffixes;
            } else {
                return {nullptr};
            }
        }

    };

    struct TermTree : BaseTree {

        std::shared_ptr<PrefixExprTree> prefixExpr{nullptr};
        std::shared_ptr<tokens::Token> token{nullptr};

        explicit TermTree(std::shared_ptr<PrefixExprTree> prefixExpr) : prefixExpr(std::move(prefixExpr)) {}

        explicit TermTree(std::shared_ptr<tokens::Token> token) : token(std::move(token)) {}

        std::string getPrintValue() override {
            std::string result = "Term";
            if (token) {
                result += " [" + *token->getType() + " : " + token->getRawValue() + "]";
            }
            return result;
        }

        std::shared_ptr<BaseTree> getLeft() override {
            return {nullptr};
        }

        std::shared_ptr<BaseTree> getRight() override {
            return prefixExpr;
        }

    };

    struct ForLoopTree : BaseTree {
        std::shared_ptr<ListTree> identifierList;
        std::shared_ptr<ListTree> expList;
        std::shared_ptr<ListTree> block;
        std::shared_ptr<tokens::Token> op;

        ForLoopTree(
                std::shared_ptr<ListTree> identifierList,
                std::shared_ptr<tokens::Token> op,
                std::shared_ptr<ListTree> expList,
                std::shared_ptr<ListTree> block
        ) : identifierList(std::move(identifierList)), expList(std::move(expList)), block(std::move(block)), op(std::move(op)) {}

        inline std::string getPrintValue() override {
            return "For [" + op->getRawValue() + "] Loop";
        }

        inline std::shared_ptr<BaseTree> getLeft() override {
            return std::make_shared<BinTree>(BinTree::Type::BINARY_OPERATION, identifierList, expList, op);
        }

        inline std::shared_ptr<BaseTree> getRight() override {
            return block;
        }

    };


}

#endif //AUX_TREE_H
