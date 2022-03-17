#include <cstdio>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" DLLEXPORT void print(double string){
    fprintf(stdout, "%g\n", string);
}
extern "C" DLLEXPORT double read(){
    double val;
    fscanf(stdin, "%lg", &val);
    return val;
}
