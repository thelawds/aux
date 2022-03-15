#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"


#include <gtest/gtest.h>

#include <string>
#include <list>

#include "glog/logging.h"
#include "../src/scanner/ModularScanner.h"
#include "../src/intermediate_representation/tree/Tree.h"
#include "../src/scanner/input_stream/PreprocessedFileInputStream.h"
#include "../src/semantics/ConstantExpressionEvaluationVisitor.h"
#include "../src/parser/Parser.h"
#include "TreePrintingVisitor.h"

#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>

using namespace std;
using namespace ogdf;
using namespace aux::scanner;
using namespace aux::scanner::input_stream;
using namespace aux::parser;
using namespace aux::ir::tokens;
using namespace aux::ir::tree;

#define COLOR_LEMON {253, 255,0}
#define COLOR_RED {255, 0, 0}

struct ParserIndexedStringStream : input_stream::IIndexedStream<char> {

    explicit ParserIndexedStringStream(const std::string &string) : _stream(
            basic_stringstream<char>{string}
    ) {}

    char get() override {
        if (!peeked) {
            ++col;
        }
        peeked = false;
        auto curr = static_cast<char>(_stream.get());

        if (curr == '\n') {
            currRow = "";
        } else {
            currRow.push_back(curr);
        }

        return curr;
    }

    char peek() override {
        if (!peeked) {
            ++col;
            peeked = true;
        }

        return _stream.peek();
    }

    void unget() override {
        _stream.unget();
        --col;
    }

    uint16_t getRow() override {
        return 0;
    }

    uint16_t getColumn() override {
        return col;
    }

    string skipToTheEndOfCurrRow() override {
        while (peek() != '\n') {
            get();
        }
        auto result = currRow;
        get();
        return result;
    }

private:
    basic_stringstream<char> _stream;
    int col{-1};
    bool peeked{false};
    string currRow;
};

TEST(ExpressionParserTest, TestSomeShit) {
//    string test = "A and B and C";
//    string test = "(TRUE and 12 * foo^bar(32) / (18 * x/y) <= 435 and 18 > 9 or bac-cab|32 > 0)";
//    string test = "foo.cat({abc = 'Stepan', def='Vasiliy'}, \"Vova\")";
//    string test = "43 + (73 - 23) * cat + 3434.32 * not 'vovanchik'^avc^12 / (32 * 23)";
//    string test = "a,b,c,d, ";
//    string test = "A = A and B and C, 132";
//    string test = "for and = 12,25+7 do end";
//    string test = "a.x.y[123+2](123+33, 9*21*b)";
    string test = "12 * 32 - 12^3 > 0 and true";
    test.push_back('\n');

    ParserIndexedStringStream stream{test};
    std::shared_ptr<ModularScanner> scanner = make_shared<ModularScanner>(stream);
    Parser parser{scanner};

    auto tree = parser.parse();
    TreePrintingVisitor treePrintingVisitor;
    treePrintingVisitor.printTree("../test/resources/ExpressionTest.svg", tree.get());
}

TEST(ParserTest, ForFile) {
    PreprocessedFileInputStream fis{"../test/resources/test_cases/Factorial.lua"};
    std::shared_ptr<ModularScanner> scanner = make_shared<ModularScanner>(fis);
    Parser fromFileParser{scanner};

    auto tree = fromFileParser.parse();
    TreePrintingVisitor treePrintingVisitor;
    treePrintingVisitor.printTree("../test/resources/FromFile.svg", tree.get());
}

TEST(SemanticsTest, ConstantExpressionEvaluation) {
    string testCase = "function test (n)\n"
                      " print (test('hello'..' world'))\n"
                      " if (4 <= 3 and 7 > 8) then return 1 + 2 + 3 else return -1 + -2 + -3 end\n"
                      "end\n"
                      "abc, ghi = {[12 + 21] = 14, x = 'string1'..\" string2\", 7^2}, 77 - 127 \n"
                      "abc[11+22] = 13";

    ParserIndexedStringStream stream{testCase};
    std::shared_ptr<ModularScanner> scanner = make_shared<ModularScanner>(stream);
    Parser parser{scanner};
    auto tree = parser.parse();

    aux::ir::semantics::ConstantExpressionEvaluationVisitor constantExpressionEvaluationVisitor;
    constantExpressionEvaluationVisitor.evalAllExpressions(tree.get());

    TreePrintingVisitor treePrintingVisitor;
    treePrintingVisitor.printTree("../test/resources/ConstantExprEval.svg", tree.get());
}

#pragma clang diagnostic pop