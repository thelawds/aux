//
// Created by miserable on 29.01.2022.
//

#ifndef AUX_ISCANNERCOMPONENT_H
#define AUX_ISCANNERCOMPONENT_H

#include "../ScanTokenResult.h"

namespace aux::scanner::components{

    struct IScannerComponent {

        [[nodiscard]]
        virtual aux::scanner::ScanTokenResult next() const = 0;

        [[nodiscard]]
        virtual bool canProcessNextToken() const = 0;

    };

}

#endif //AUX_ISCANNERCOMPONENT_H
