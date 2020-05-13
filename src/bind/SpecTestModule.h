//
// Created by Keith on 2020/1/26.
//

#ifndef TWA_SPECTESTMODULE_H
#define TWA_SPECTESTMODULE_H

#include "../Runtime.h"

namespace TWA {

class SpecTestModule {
public:
    static Ptr<ModuleInstance> registerModule(Runtime &rt);
};

}
#endif //TWA_SPECTESTMODULE_H
