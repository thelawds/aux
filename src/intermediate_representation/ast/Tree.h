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
            LIST_ELEM
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
                    {Type::NONE,                    "None"},
                    {Type::TABLE_FIELD_DECLARATION, "Table Field Declaration"},
                    {Type::BINARY_OPERATION,        "Binary Operation"},
                    {Type::UNARY_OPERATION,         "Unary Operation"},
                    {Type::LIST_ELEM,               "List Element"}
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
            PE_SUFFIX_LIST
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
                    {Type::NONE,             "None List"},
                    {Type::EXPRESSION_LIST,  "Expression List"},
                    {Type::TABLE_FIELD_LIST, "Table Field List"},
                    {Type::PE_SUFFIX_LIST,   "Prefix Expression\nSuffix List"}
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
        std::shared_ptr<tokens::Token> token;

        explicit TokenTree(std::shared_ptr<tokens::Token> token) : token(std::move(token)) {}

        inline std::string getPrintValue() override {
            return *token->getType() + " : " + token->getRawValue();
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
            return "Function Call Arguments\n"
                   "       [  =  ]";
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

}

#endif //AUX_TREE_H
