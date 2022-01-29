#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "../src/scanner/components/numeric/NumericConstantsDFSAScanner.h"

using namespace std;
using namespace aux::scanner;
using namespace aux::ir::tokens;
using namespace aux::scanner::components::numeric;


TEST(ScannerTest, NumericConstantsScannerTest){
    string str = "0xAB12+";
    stringstream stream(str);

    NumericConstantsDFSAScanner scanner{stream};
    auto result = scanner.next();

    EXPECT_TRUE(result);
}
