#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"


#include <gtest/gtest.h>

#include <string>
#include <list>

#include "glog/logging.h"
#include "../src/scanner/ModularScanner.h"
#include "../src/scanner/input_stream/PreprocessedFileInputStream.h"
#include "../src/parser/Parser.h"

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
using namespace aux::ir::ast;

#define COLOR_LEMON {253, 255,0}
#define COLOR_RED {255, 0, 0}

struct ParserIndexedStringStream: input_stream::IIndexedStream<char> {

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

void makeGraph(Graph &G, GraphAttributes &GA, node currNode, shared_ptr<BaseTree> tree){
    GA.label(currNode) = tree->getPrintValue();
    GA.shape(currNode) = ogdf::Shape::Ellipse;
    GA.width(currNode) = 150;
    GA.height(currNode) = 150;

    if (tree->getLeft()) {
        node left = G.newNode();
        edge toLeft = G.newEdge(currNode, left);
        GA.strokeColor(toLeft) = COLOR_LEMON;
        GA.strokeWidth(toLeft) = 5.f;
        makeGraph(G, GA, left, tree->getLeft());
    }

    if (tree->getRight()) {
        node right = G.newNode();
        edge toRight = G.newEdge(currNode, right);
        GA.strokeColor(toRight) = COLOR_RED;
        GA.strokeWidth(toRight) = 5.f;
        makeGraph(G, GA, right, tree->getRight());
    }
}

void drawGraph(const shared_ptr<BaseTree> &tree){
    Graph G;
    GraphAttributes GA(
            G, GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics
               | GraphAttributes::nodeLabel | GraphAttributes::nodeStyle
               | GraphAttributes::edgeStyle
    );

    makeGraph(G, GA, G.newNode(), tree);

    SugiyamaLayout SL;
    SL.setRanking(new OptimalRanking);
    SL.setCrossMin(new MedianHeuristic);

    auto *ohl = new OptimalHierarchyLayout;
    ohl->layerDistance(50.0);
    ohl->nodeDistance(100.0);
    ohl->weightBalancing(0.8);
    SL.setLayout(ohl);

    SL.call(GA);

    GraphIO::write(GA, "../test/resources/tree.svg", GraphIO::drawSVG);

}

TEST(ExpressionParserTest, TestSomeShit) {
//    string test = "A and B and C";
//    string test = "TRUE and 12 * foo^bar(32) / (18 * x/y[15]) <= 435 and 18 > 9 or bac-cab|32 > 0";
//    string test = "foo.cat({abc = 'Stepan', def='Vasiliy'}, \"Vova\")";
//    string test = "43 + (73 - 23) * cat + 3434.32 * not 'vovanchik'^avc^12 / (32 * 23)";
//    string test = "a,b,c,d, ";
//    string test = "A = A and B and C, 132";
//    string test = "for and = 12,25+7 do end";
//    string test = "a.x.y[123+2](123+33, 9*21*b)";
    string test = "if a-b>0 then\n"
                  "    if x-y > 0 then\n"
                  "         return 12\n"
                  "     end\n"
                  "else return 25 end ";
    test.push_back('\n');
    ParserIndexedStringStream stream{test};
    std::shared_ptr<ModularScanner> scanner = make_shared<ModularScanner>(stream);
    Parser parser{scanner};

    auto tree = parser.parse();
    drawGraph(tree);
}

TEST(ParserTest, ForFile){
    PreprocessedFileInputStream fis{"../test/resources/test_cases/Factorial.lua"};
    std::shared_ptr<ModularScanner> scanner = make_shared<ModularScanner>(fis);
    Parser fromFileParser{scanner};

    auto tree = fromFileParser.parse();
    drawGraph(tree);
}

#pragma clang diagnostic pop