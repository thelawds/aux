//
// Created by miserable on 30.01.2022.
//

#ifndef AUX_OPERATORSSCANNER_H
#define AUX_OPERATORSSCANNER_H

#include "IScannerComponent.h"

namespace aux::scanner::components {

    struct OperatorsScanner : IScannerComponent {

        ScanTokenResult next(ir::tokens::Span span) const override;

        bool canProcessNextToken() const override;

    private:

    };

}



#endif //AUX_OPERATORSSCANNER_H
