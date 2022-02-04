//
// Created by miserable on 04.02.2022.
//

#ifndef AUX_LEXICALANALYSISERRORMESSAGES_H
#define AUX_LEXICALANALYSISERRORMESSAGES_H

#include "glog/logging.h"
#include <string>

inline std::string MODULAR_SCANNER_GENERAL_ERROR(char c1, int i1, int i2){
    return "Fatal Error. Was not able to scan token starting with [" + std::string(1, c1)
           + "] at ("
           + std::to_string(i1)
           + ":"
           + std::to_string(i2)
           + "). Exiting.";
};

inline std::string MODULAR_SCANNER_ERROR(int i1, int i2, const std::string& s1){
    return "Error scanning file at (" + std::to_string(i1) + ":" + std::to_string(i2) + "). Error message is: " + s1;
}

#endif //AUX_LEXICALANALYSISERRORMESSAGES_H
