//
// Created by miserable on 08.02.2022.
//

#include "../src/util/Defines.h"

#include <gtest/gtest.h>
#include <cmath>

#include <string>
#include <iostream>
#include <vector>
#include <list>

#include "glog/logging.h"
#include "../src/scanner/ModularScanner.h"
#include "../src/parser/Parser.h"

using namespace std;
using namespace aux::scanner;
using namespace aux::parser;
using namespace aux::ir::tokens;

struct IndexedStringStream : input_stream::IIndexedStream<CommonCharType> {

    explicit IndexedStringStream(const CommonStringType &string) : _stream(
            basic_stringstream<CommonCharType>{string}
    ) {}

    CommonCharType get() override {
        auto curr = static_cast<CommonCharType>(_stream.get());
        return curr;
    }

    CommonCharType peek() override {
        return static_cast<CommonCharType>(_stream.peek());
    }

    void unget() override {
        _stream.unget();
    }

    uint16_t getRow() override {
        return 0;
    }

    uint16_t getColumn() override {
        return 0;
    }

private:
    basic_stringstream<CommonCharType> _stream;
};

template<typename _AST_Tree>
int calcDepth(const shared_ptr<_AST_Tree> &tree) {
    int depth = 1;

    if (tree->left == nullptr && tree->right == nullptr) {
        return depth;
    }

    int ldepth = 0;
    if (tree->left) {
        ldepth += calcDepth(tree->left);
    }

    int rdepth = 0;
    if (tree->right) {
        rdepth += calcDepth(tree->right);
    }


    return depth + max(ldepth, rdepth);

}

string make8Sym(const string &s) {
    string res = "[";
    for (int i = 0; i < min(8, int(s.length())); ++i) {
        res += s[i];
    }
    res += "]";
    for (size_t i = res.size() - 1; i < 8; ++i) {
        res += " ";
    }
    return res;
}

int64_t pow(int64_t a, int64_t b) {
    int64_t res = 1;
    while (b-- >= 0) {
        res *= a;
    }
    return res;
}

template<typename _AST_Tree>
void printTree(const shared_ptr<_AST_Tree> &tree) {
    int64_t depth = calcDepth(tree) - 1;
    int64_t size = ::pow(2, depth + 1) + 1;
    list<tuple<int64_t, int64_t, int64_t, shared_ptr<_AST_Tree>>> trees;
    vector<vector<string>> result(size);
    for (int64_t i = 0; i < size; ++i) {
        result[i].resize(size);
        for (int64_t j = 0; j < size; ++j) {
            result[i][j] = "          ";
        }
    }


    trees.emplace_back(size / 2, 0, ::pow(2, depth - 1), tree);
    while (!trees.empty()) {
        auto[row, col, shiftChange, currTree] = trees.front();
        trees.pop_front();

        if (currTree->right) {
            trees.emplace_back(row - shiftChange, col + 1, shiftChange / 2, currTree->right);
            for (int64_t i = row; i > row - shiftChange; --i) {
                result[i][col] = string{" |        "};
            }
            result[row - shiftChange][col] = " /------->";
        }
        if (currTree->left) {
            trees.emplace_back(row + shiftChange, col + 1, shiftChange / 2, currTree->left);
            for (int64_t i = row; i < row + shiftChange; ++i) {
                result[i][col] = string{" |        "};
            }
            result[row + shiftChange][col] = " \\------->";
        }

        if (currTree->op) {
            result[row][col] = make8Sym(*static_pointer_cast<TokenOperator>(currTree->op)->getOperator());
        } else if (currTree->value) {
            switch (currTree->value->getType()) {
                case TokenType::NUMERIC_DECIMAL:
                    result[row][col] = make8Sym(static_pointer_cast<TokenDecimal>(currTree->value)->getRawValue());
                    break;
                case TokenType::NUMERIC_HEX:
                    result[row][col] = make8Sym(static_pointer_cast<TokenHex>(currTree->value)->getRawValue());
                    break;
                case TokenType::NUMERIC_DOUBLE:
                    result[row][col] = make8Sym(static_pointer_cast<TokenDouble>(currTree->value)->getRawValue());
                    break;
                default:
                    result[row][col] = make8Sym("Undef.");
                    break;
            }

        }

    }

    ofstream out{"../test/resources/printedTree.txt"};

    for (const auto &row: result) {
        for (const auto &col: row) {
            out << col;
        }
        out << endl;
    }

}

TEST(ExpressionParserTest, TestSomeShit) {
//    string test = "12 * foo^bar(32) / (18 * (x/y[15]))";
    string test = "foo.cat()";
    IndexedStringStream stream{test};
    std::shared_ptr<ModularScanner> scanner = make_shared<ModularScanner>(stream);
    Parser exprParser{scanner};

    auto tree = exprParser.parse();
    auto res = static_pointer_cast<aux::ir::ast::PrefixExprTree>(tree);
    cout << "Done";
//    printTree(tree);
}
