//
// Created by miserable on 28.01.2022.
//

#ifndef AUX_DEFINES_H
#define AUX_DEFINES_H

#define IMPLICIT // specifically to perform implicit type conversion

template<typename InputType>
using Predicate = bool (*)(InputType);

template<typename IN, typename OUT>
using Function = OUT (*)(IN);

#endif //AUX_DEFINES_H
