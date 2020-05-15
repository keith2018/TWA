//
// Created by Keith on 2020/1/26.
//

#pragma once

#include "../runtime.h"

namespace TWA {

class SpecTestModule {
public:
    static Ptr<ModuleInstance> registerModule(Runtime &rt);
};

}
