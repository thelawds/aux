#include <gflags/gflags.h>
#include <glog/logging.h>

#include "scanner/ModularScanner.h"
#include "scanner/input_stream/PreprocessedFileInputStream.h"
#include "parser/Parser.h"
//#include "semantics/CodeGenerationVisitor.h"
#include "semantics/transformation/AstToProgramTreeTransformationVisitor.h"
#include "semantics/codegen/CodeGenerationVisitor.h"

DEFINE_string(src, "", "Source file to be compiled");

int main(int argc, char** argv) {
    std::string usage = "This is aux lua compiler. Sample usage:\n";
    usage += std::string(argv[0]) + " [options]";
    gflags::SetUsageMessage(usage);

    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    if (FLAGS_src.empty()) {
        LOG(FATAL) << "Input file is not provided. See usage:";
    }

    aux::scanner::input_stream::PreprocessedFileInputStream fileInputStream(FLAGS_src);
    auto scanner = std::make_shared<aux::scanner::ModularScanner>(fileInputStream);
    auto parser = std::make_shared<aux::parser::Parser>(scanner);
    aux::semantics::AstToProgramTreeTransformationVisitor transformationVisitor;
    aux::semantics::codegen::CodeGenerationVisitor codeGenerationVisitor;

    auto syntaxTree = parser->parse();
    auto programTree = transformationVisitor.transform(syntaxTree.get());
    codeGenerationVisitor.generateLLVMIr(programTree);

    return 0;
}
