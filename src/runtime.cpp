//
// Created by Keith on 2020/1/8.
//

#include "runtime.h"
#include <iostream>
#include "opcode.h"
#include "operation.h"

namespace TWA {

Runtime::Runtime(U32 stackSize) {
    trap_ = TrapNone;
    exitCode_ = 0;
    stack_.create(stackSize);

    for (int i = 0; i < OpMaxLength; i++) {
        execOpFunc[i] = nullptr;
    }

    REG_ALL_OP_FUNC
}

Runtime::~Runtime() {
    stack_.release();
}

bool Runtime::invoke(TypedValue &result, Addr &addr, std::vector<Value> &params) {
    FuncInstance &funcInst = *store_.funcs[addr];
    return invoke(result, funcInst, params);
}

bool Runtime::invoke(TypedValue &result, FuncInstance &funcInst, std::vector<Value> &params) {
    if (params.size() != funcInst.type.params.size) {
        std::cout << "params length not equal\n";
        return false;
    }

    // clear states
    trap_ = TrapNone;
    exitCode_ = 0;
    stack_.reset();
    pc_ = ProgramCounter();

    stack_.pushDummyFrame();

    for (U32 idx = 0; idx < params.size(); idx++) {
        stack_.pushValue(params[idx]);
    }

    bool invokeOk = invoke(funcInst);
    if (invokeOk) {
        invokeOk = execInstruction();
        if (invokeOk) {
            Value val;
            if(funcInst.type.retType != ValueTypeVoid) {
                if (stack_.values.empty()) {
                    std::cout << "result value empty\n";
                    return false;
                }
                val = stack_.popValue();
            }
            result = TypedValue(funcInst.type.retType, val);
        } else {
            std::cout << "Error: [trap] " << TrapDesc[trap_] << "\n";
            if (trap_ != TrapExit) {
                exitCode_ = trap_;
            }
        }
    }

    return invokeOk;
}

bool Runtime::invoke(FuncInstance &funcInst) {
    if (unlikely(funcInst.native)) {
        return funcInst.nativeCall(*this);
    } else {
        stack_.pushFrame(pc_, funcInst);
        Frame *frame = &stack_.frames.top();

        // frame locals
        U32 paramsSize = funcInst.type.params.size;
        U32 funcLocalSize = funcInst.localCnt;
        frame->localSize = paramsSize + funcLocalSize;
        if (likely(frame->localSize > 0)) {
            if (unlikely(stack_.locals.full(frame->localSize))) {
                std::cout << "locals stack overflow\n";
                trap_ = TrapStackOverflow;
                return false;
            }

            frame->locals = stack_.locals.data() + stack_.locals.size();
            stack_.locals.sizeIncrease(frame->localSize);
            for (int idx = paramsSize - 1; idx >= 0; idx--) {
                frame->locals[idx] = stack_.popValue();
            }
            for (U32 idx = paramsSize; idx < frame->localSize; idx++) {
                frame->locals[idx] = Value();
            }
        }

        if (unlikely(stack_.frames.full())) {
            trap_ = TrapStackOverflow;
            return false;
        }

        // run
        pc_.set(frame->fbi.ins, 2, 0);
        return true;
    }
}

bool Runtime::invokeStartFunc(Addr &addr) {
    // [] -> []
    std::vector<Value> params;
    TypedValue result;
    return invoke(result, addr, params);
}

bool Runtime::execInstruction() {
    bool execOk = true;
    while (execOk) {
        if (unlikely(pc_.dummy())) {
            execOk = true;
            break;
        }

        if (unlikely(pc_.end())) {
            std::cout << "end of pc\n";
            execOk = false;
            break;
        }

        BaseInstr *instrPtr = pc_.nextInstr();

        if (unlikely(execOpFunc[instrPtr->opCode] == nullptr)) {
            char opCodeStr[5];
            sprintf(opCodeStr, "0x%02x", instrPtr->opCode);
            std::cout << "OpCode not support: " << opCodeStr << "\n";
            execOk = false;
            break;
        }

        execOk = (execOpFunc[instrPtr->opCode])(instrPtr, *this);
    }

    return execOk;
}

}