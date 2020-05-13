//
// Created by Keith on 2019/12/25.
//

#ifndef TWA_INSTANTIATION_H
#define TWA_INSTANTIATION_H

#include "Store.h"
#include "Module.h"

namespace TWA {

class Runtime;

class Instantiation {
public:
    static bool instantiate(Ptr<ModuleInstance> &moduleInst,
                     Runtime &runtime,
                     Module &module,
                     std::vector<ExternalValAddr> &extVal);

private:
    static Value evaluateInitValue(ExprCodes &expr);
};

}
#endif //TWA_INSTANTIATION_H
