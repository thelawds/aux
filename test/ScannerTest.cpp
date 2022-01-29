#include <gtest/gtest.h>

#include <string>
#include <vector>
#include "../src/scanner/components/numeric/NumericConstantsDFSAScanner.h"

using namespace std;
using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::scanner::components::numeric;


TEST(ScannerTest, NumericConstantsScannerPositiveTest) {
    vector<char> delimiters = {
            '+', '-', '*', '/', '(', ')', '[', ' ', '\t'
    };
    vector<string> inputs{
            "0", "3", "345", "0xff", "0xBEBADA",
            "3.0", "3.1416", "314.16e-2", "0.31416E1", "34e1",
            "0x0.1E", "0xA23p-4", "0X1.921FB54442D18P+1"
    };

    string str;
    for (const string &input: inputs) {
        size_t i = random() % delimiters.size();
        str += input;
        str += delimiters[i];
    }

    stringstream stream(str);
    NumericConstantsDFSAScanner scanner{stream};
    while (stream.peek() != -1) {
        auto result = scanner.next();
        EXPECT_TRUE(result);
        stream.get();
    }

}

TEST(ScannerTest, NumericConstantsScannerNegativeTest) {
    vector<string> inputs{
            "0e0",
            ".x",
            ",",
            "1x0"
    };

    for (const auto &str: inputs) {
        stringstream stream(str);
        NumericConstantsDFSAScanner scanner{stream};
        auto result = scanner.next();
        EXPECT_FALSE(result);
    }
}
