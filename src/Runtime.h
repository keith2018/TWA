//
// Created by Keith on 2020/1/8.
//

#ifndef TWA_RUNTIME_H
#define TWA_RUNTIME_H

#include "Store.h"
#include "Stack.h"
#include "Utils.h"

namespace TWA {

class Runtime {
public:
    Runtime(U32 stackSize = 1024);
    ~Runtime();

    inline Store &store() { return store_; }

    bool invoke(TypedValue &result, Addr &addr, std::vector<Value> &params);
    bool invoke(TypedValue &result, FuncInstance &funcInst, std::vector<Value> &params);
    bool invokeStartFunc(Addr &addr);
    bool invoke(FuncInstance &funcInst);

    inline void *getCurrMem(U32 offset) {
        return store_.mems[stack_.frames.top().module->memAddrs[0]]->data + offset;
    }

private:
    bool execInstruction();

public:
    RuntimeTrap trap_;
    U32 exitCode_;
    ProgramCounter pc_;
    Stack stack_;
    Store store_;
};

}
#endif //TWA_RUNTIME_H
