//
// Created by miserable on 22.03.2022.
//

#ifndef AUX_SYMBOLTABLE_H
#define AUX_SYMBOLTABLE_H

#include <string>
#include <stdexcept>
#include <map>

namespace aux::ir {

    template<typename T>
    struct SymbolTable {

        inline bool contains(const std::string &name){
            return values.contains(name);
        }

        inline T find(const std::string &name) {
            if (values.contains(name)) {
                return values[name];
            } else {
                throw std::logic_error("No value for " + name);
            }
        }

        inline void add(const std::string &name, T value) {
            values[name] = value;
        }

        inline std::map<std::string, T>& getInternal(){
            return values;
        }

    private:

        std::map<std::string, T> values;

    };

}


#endif //AUX_SYMBOLTABLE_H
