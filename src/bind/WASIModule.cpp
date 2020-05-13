//
// Created by Keith on 2020/1/20.
//

#include <iostream>
#include "../Native.h"
#include "WASIModule.h"
#include "WASITypes.h"

namespace TWA {
namespace WASI {

#define PUSH_RET_I32(val)       \
    Value ret((I32)val);        \
    rt.stack_.pushValue(ret);   \

Ptr<ModuleInstance> WASIModule::registerModule(Runtime &rt) {
    Ptr<ModuleInstance> moduleInst = Ptr<ModuleInstance>(new ModuleInstance());
    std::vector<Ptr<FuncInstance>> &funcs = rt.store().funcs;

    // environ_sizes_get
    REG_NATIVE_FUNCTION_BEGIN(environ_sizes_get) {
            Value envCount = rt.stack_.popValue();
            Value envBufSize = rt.stack_.popValue();

            // TODO
            *(Wasi_size_t *) rt.getCurrMem(envCount.i32) = 0;
            *(Wasi_size_t *) rt.getCurrMem(envBufSize.i32) = 0;

            PUSH_RET_I32(success)
            return true;
        }
    REG_NATIVE_FUNCTION_END(environ_sizes_get)

    // environ_get
    REG_NATIVE_FUNCTION_BEGIN(environ_get) {
            Value env = rt.stack_.popValue();
            Value envBuf = rt.stack_.popValue();

            // TODO

            PUSH_RET_I32(success)
            return true;
        }
    REG_NATIVE_FUNCTION_END(environ_get)

    // args_sizes_get
    REG_NATIVE_FUNCTION_BEGIN(args_sizes_get) {
            Value argc = rt.stack_.popValue();
            Value argvBufSize = rt.stack_.popValue();

            // TODO
            *(Wasi_size_t *) rt.getCurrMem(argc.i32) = 0;
            *(Wasi_size_t *) rt.getCurrMem(argvBufSize.i32) = 0;

            PUSH_RET_I32(success)
            return true;
        }
    REG_NATIVE_FUNCTION_END(args_sizes_get)

    // args_get
    REG_NATIVE_FUNCTION_BEGIN(args_get) {
            Value argv = rt.stack_.popValue();
            Value argvBuf = rt.stack_.popValue();

            // TODO

            PUSH_RET_I32(success)
            return true;
        }
    REG_NATIVE_FUNCTION_END(args_get)

    // proc_exit
    REG_NATIVE_FUNCTION_BEGIN(proc_exit) {
            Value eCode = rt.stack_.popValue();
            rt.trap_ = TrapExit;
            rt.exitCode_ = eCode.i32;
            return false;
        }
    REG_NATIVE_FUNCTION_END(proc_exit)

    return moduleInst;
}


}
}