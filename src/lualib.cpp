#include <cstdio>
#include <string>
#include <stdexcept>
#include <map>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

enum Type {
    INTEGER = 0,
    STRING = 1,
    FLOAT = 2,
    NIL = 3,
    TABLE = 4
};

struct T {
    Type type;
    char *value;

//    T(Type type, char *value) : type(type), value(value) {}

};

/**
 * <b> Functions working with types </b>
 */

using TableType = std::map<std::string, T>;

extern "C" DLLEXPORT double __toFloat__(T value);
extern "C++" DLLEXPORT std::string __toString__(T value);

extern "C" DLLEXPORT T __getTable__() {
    TableType *table = new TableType();
    return {TABLE, (char *) table};
}

std::string __toStringForTable__(T value){
    switch (value.type) {
        case INTEGER:
            return "INT_" + __toString__(value);
        case STRING:
            return "STRING_" + __toString__(value);
        case FLOAT:
            return "INT_" + std::to_string((long long) __toFloat__(value));
        default:
            throw std::logic_error("Tables can only be referenced by integer expressions and strings");
    }
}

std::string __fromStringForTable__(std::string str){
    if (str.substr(0, 4) == "INT_") {
        return str.substr(4);
    } else if (str.substr(0, 7) == "STRING_") {
        return str.substr(7);
    } else {
        return str;
    }
}

extern "C" DLLEXPORT T* __getTableFieldPtr__(T& table, T fieldReference){

    if (table.type != TABLE) {
        throw std::logic_error("Error accessing with []. Not a table"); // todo
    }

    TableType *internalTable = (TableType *) table.value;
    std::string reference = __toStringForTable__(fieldReference);

    if (internalTable->find(reference) == internalTable->end()) {
        (*internalTable)[reference] = {NIL, nullptr};
    }

    return &internalTable->at(reference);
}

extern "C" DLLEXPORT T __getTableFieldValue__(T table, T fieldReference) {
    return *__getTableFieldPtr__(table, fieldReference);
}

extern "C" DLLEXPORT void __putField__(T table, T fieldReference, T fieldValue){
    if (table.type != TABLE) {
        throw std::logic_error("Error accessing with []. Not a table"); // todo
    }

    TableType *internalTable = (TableType *) table.value;
    std::string reference = __toStringForTable__(fieldReference);
    (*internalTable)[reference] = fieldValue;
}

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
            throw std::logic_error("Conversion from " + type + " to double can not be performed");
    }
}

extern "C++" DLLEXPORT std::string __toString__(T value) {
    switch (value.type) {
        case INTEGER:
            return std::to_string(*((long long *) value.value));
        case STRING:
            return value.value;
        case FLOAT:
            return std::to_string(__toFloat__(value));
        case NIL:
            return "nil";
        default:
            throw std::logic_error("Conversion from struct to string can not be performed"); // todo: make structs printable
    }
}

/**
 * <b> Standard Library Functions </b>
 */

extern "C" DLLEXPORT void __print__(T string) {
    // todo: change printing
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
        case TABLE: {
            fprintf(stdout, "Table {");
            bool needComma = false;
            for (auto field: *((TableType *) string.value)) {
                if (needComma) {
                    fprintf(stdout, ", ");
                }
                auto &name = field.first;
                auto &value = field.second;
                fprintf(stdout, "%s: ", __fromStringForTable__(name).c_str());
                __print__(value);
                needComma = true;
            }
            fprintf(stdout, "}");
            break;
        }
        case NIL:
            fprintf(stdout, "%s\n", "NIL");
            break;
    }
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