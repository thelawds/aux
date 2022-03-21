#include <cstdio>
#include <string>
#include <stdexcept>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

enum Type {
    INTEGER = 0,
    STRING = 1,
    STRUCT = 2,
    FLOAT = 3,
    NIL = 4
};

struct T {
    Type type;
    char *value;

//    T(Type type, char *value) : type(type), value(value) {}

};

/**
 * <b> Functions working with types </b>
 */

extern "C" DLLEXPORT T __getBoolean__(bool b) {
    long long *b2 = new long long(b);
    return {INTEGER, (char *) b2};
}

extern "C" DLLEXPORT T __getInteger__(long long value) {
    long long *valPtr = new long long(value);
    return {INTEGER, (char *) valPtr};
}


extern "C" DLLEXPORT T __getFloat__(double value) {
    double *valPtr = new double(value);
    return {FLOAT, (char *) valPtr};
}

extern "C" DLLEXPORT T __getString__(char *string){
    return {STRING, string};
}

extern "C" DLLEXPORT double __toFloat__(T value) {
    switch (value.type) {
        case INTEGER:
            return *((long long *) value.value);
        case STRING:
            return std::stod(value.value);
        case FLOAT:
            return *((double *) value.value);
        default:
            std::string type = value.type == NIL ? "nil" : "type struct";
            fprintf(stderr, "Conversion from %s to double can not be performed", type.c_str());
            throw std::logic_error("Conversion from " + type + " to double can not be performed");
    }
}

/**
 * <b> Standard Library Functions </b>
 */

extern "C" DLLEXPORT void __print__(T string) {
    switch (string.type) {
        case INTEGER:
            fprintf(stdout, "%lld", *((long long *) string.value));
            break;
        case STRING:
            fprintf(stdout, "%s", string.value);
            break;
        case FLOAT:
            fprintf(stdout, "%g", *((double *) string.value));
            break;
        case STRUCT:
            return; //todo
        case NIL:
            fprintf(stdout, "%s", "NIL");
            break;
    }
    fprintf(stdout, "\n"); // todo remove
}

extern "C" DLLEXPORT T __read__() {
    double *val = new double(0.0);
    fscanf(stdin, "%lg", val);

    return {FLOAT, (char *) val};
}

extern "C" void __start__();

int main(int argc, char **argv) {
    __start__();
    return 0;
}