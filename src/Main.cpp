#include <gflags/gflags.h>
#include <glog/logging.h>

#include "scanner/ModularScanner.h"
#include "scanner/input_stream/PreprocessedFileInputStream.h"

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

    aux::scanner::input_stream::PreprocessedFileInputStream fis(FLAGS_src);
    aux::scanner::ModularScanner scanner(fis);

    while (true) {
        auto currToken = scanner.next();
        if (currToken) {
            LOG(INFO) << "Token of type " << *currToken->getType()
                      << " at (" << currToken->getSpan().row << " : " << currToken->getSpan().column << ")";
        } else {
            break;
        }
    }

    return 0;
}
