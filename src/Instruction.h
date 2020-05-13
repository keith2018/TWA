//
// Created by Keith on 2019/12/20.
//

#ifndef TWA_INSTRUCTION_H
#define TWA_INSTRUCTION_H

#include "TWA.h"
#include "OpCode.h"
#include <vector>

namespace TWA {

struct BaseInstr {
    explicit BaseInstr(U8 oc = Op_unreachable) {
        opCode = oc;
    }

    U8 opCode;
};

struct ExprCodes {
    BaseInstr **codes = nullptr;
    U32 size = 0;
};

struct CtrlBaseInstr : BaseInstr {
}; // unreachable, nop, return, else

struct CtrlBlockInstr : BaseInstr {
    ExprCodes expr;
    U32 elseIdx;
    ValueType resultType;
}; // block, loop, if

struct CtrlBrInstr : BaseInstr {
    U32 labelIdx;
}; // br, br_if

struct CtrlBrTableInstr : CtrlBrInstr {
    std::vector<U32> labelVec;
}; // br_table

struct CtrlCallInstr : BaseInstr {
    U32 idx;
}; // call, call_indirect

struct ParametricInstr : BaseInstr {
};

struct VariableInstr : BaseInstr {
    U32 idx;
};

struct MemoryInstr : BaseInstr {
    U32 align;
    U32 offset;
};

struct NumericInstr : BaseInstr {
};

struct NumericConstInstr : BaseInstr {
    Value val;
};

void releaseInstr(BaseInstr *ins);
void releaseExprCodes(ExprCodes &ec);

}
#endif //TWA_INSTRUCTION_H
