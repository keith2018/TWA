//
// Created by Keith on 2020/2/1.
//

#include "Instruction.h"

namespace TWA {

void releaseExprCodes(ExprCodes &ec){
    for (U32 i = 0; i < ec.size; i++) {
        releaseInstr(ec.codes[i]);
    }
    delete[] ec.codes;
    ec.codes = nullptr;
    ec.size = 0;
}

void releaseInstr(BaseInstr *ins) {
    if (ins->opCode == Op_block || ins->opCode == Op_loop || ins->opCode == Op_if) {
        CtrlBlockInstr* blockInstr = (CtrlBlockInstr*)ins;
        releaseExprCodes(blockInstr->expr);
    }
    delete ins;
}

}