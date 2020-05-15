//
// Created by Keith on 2019/12/25.
//

#pragma once

#include "store.h"
#include "module.h"

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
