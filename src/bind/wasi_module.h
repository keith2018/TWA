//
// Created by Keith on 2020/1/20.
//

#pragma once


#include "../runtime.h"

namespace TWA {
namespace WASI {

class WASIModule {
public:
    static Ptr<ModuleInstance> registerModule(Runtime &rt);
};

}
}
