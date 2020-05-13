//
// Created by Keith on 2020/1/20.
//

#ifndef TWA_WASIMODULE_H
#define TWA_WASIMODULE_H


#include "../Runtime.h"

namespace TWA {
namespace WASI {

class WASIModule {
public:
    static Ptr<ModuleInstance> registerModule(Runtime &rt);
};

}
}

#endif //TWA_WASIMODULE_H
