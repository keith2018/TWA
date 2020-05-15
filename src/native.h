//
// Created by Keith on 2020/1/26.
//

#pragma once

namespace TWA {

#define REG_NATIVE_FUNCTION_BEGIN(name)                                                             \
struct FuncInstance_##name : public FuncInstance {                                                  \
    explicit FuncInstance_##name(ModuleInstance *m) : FuncInstance (m, true) {}                     \
    bool nativeCall(Runtime &rt)                                                                    \


#define REG_NATIVE_FUNCTION_END(name)                                                               \
    };                                                                                              \
                                                                                                    \
    funcs.push_back(Ptr<FuncInstance>(new FuncInstance_##name(moduleInst.get())));                  \
    moduleInst->types.push_back(funcs.back()->type);                                                \
    moduleInst->funcAddrs.emplace_back(funcs.size() - 1);                                           \
    moduleInst->exports[#name] = ExportInstance(#name, ExtTypeFunc, moduleInst->funcAddrs.back());  \

}
