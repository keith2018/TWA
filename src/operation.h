//
// Created by Keith on 2020/1/10.
//

#pragma once

#include <cmath>
#include "opcode.h"
#include "instruction.h"
#include "runtime.h"


namespace TWA {

#define REG_OP_FUNC(opcode) execOpFunc[opcode] = exec_##opcode
#define OP_FUNC_DEFINE(opcode) bool exec_##opcode(BaseInstr *instrPtr, Runtime &rt)

static bool (*execOpFunc[OpMaxLength])(BaseInstr *, Runtime &);

OP_FUNC_DEFINE(Op_unreachable) {
    rt.trap_ = TrapUnreachable;
    return false;
}

OP_FUNC_DEFINE(Op_nop) {
    return true;
}

OP_FUNC_DEFINE(Op_block) {
    CtrlBlockInstr *ptr = (CtrlBlockInstr *) instrPtr;
    rt.stack_.pushLabel(ptr->resultType, rt.pc_, rt.pc_);
    if (unlikely(rt.stack_.labels.full())) {
        rt.trap_ = TrapStackOverflow;
        return false;
    }
    rt.pc_.set(&ptr->expr, 0);
    return true;
}

OP_FUNC_DEFINE(Op_loop) {
    CtrlBlockInstr *ptr = (CtrlBlockInstr *) instrPtr;
    ProgramCounter continuation = rt.pc_;
    rt.stack_.pushLabel(ValueTypeVoid, rt.pc_, continuation.backward());
    if (unlikely(rt.stack_.labels.full())) {
        rt.trap_ = TrapStackOverflow;
        return false;
    }
    rt.pc_.set(&ptr->expr, 0);
    return true;
}

OP_FUNC_DEFINE(Op_if) {
    Value c = rt.stack_.popValue();

    CtrlBlockInstr *ptr = (CtrlBlockInstr *) instrPtr;
    rt.stack_.pushLabel(ptr->resultType, rt.pc_, rt.pc_);
    if (unlikely(rt.stack_.labels.full())) {
        rt.trap_ = TrapStackOverflow;
        return false;
    }
    if (c.i32 != 0) {
        rt.pc_.set(&ptr->expr, 0);
    } else {
        if (ptr->elseIdx >= ptr->expr.size) { // else not exist
            rt.pc_.set(&ptr->expr, ptr->expr.size - 1);
        } else {
            rt.pc_.set(&ptr->expr, ptr->elseIdx + 1);
        }
    }

    return true;
}

OP_FUNC_DEFINE(Op_end) {
    // block end
    if (likely(!rt.stack_.labels.empty())) {
        Label *label = &rt.stack_.labels.top();
        rt.pc_ = label->pre;
        rt.stack_.popLabel();
    }

    // function end
    if (rt.stack_.frames.top().labelStackSize == rt.stack_.labels.size()) {
        Frame *frame = &rt.stack_.frames.top();
        rt.pc_ = frame->pre;

        rt.stack_.locals.sizeDecrease(frame->localSize);
        rt.stack_.popFrame();
    }

    return true;
}

bool op_break_impl(Runtime &rt, U32 labelIdx) {
    // n-th L
    Label *label = &rt.stack_.labels.top(labelIdx);

    // pop value
    Value retVal;
    if (label->retType != ValueTypeVoid) {
        retVal = rt.stack_.popValue();
    }

    // clear labels & values
    rt.stack_.popLabel(labelIdx + 1);
    if (rt.stack_.values.size() > label->valueStackSize) {
        rt.stack_.popValue(rt.stack_.values.size() - label->valueStackSize);
    }

    // push back value
    if (label->retType != ValueTypeVoid) {
        rt.stack_.pushValue(retVal);
    }

    rt.pc_ = label->next;
    return true;
}

OP_FUNC_DEFINE(Op_br) {
    CtrlBrInstr *ptr = (CtrlBrInstr *) instrPtr;
    return op_break_impl(rt, ptr->labelIdx);
}

OP_FUNC_DEFINE(Op_br_if) {
    Value c = rt.stack_.popValue();
    if (c.i32 != 0) {
        CtrlBrInstr *ptr = (CtrlBrInstr *) instrPtr;
        return op_break_impl(rt, ptr->labelIdx);
    }
    return true;
}

OP_FUNC_DEFINE(Op_br_table) {
    CtrlBrTableInstr *tablePtr = (CtrlBrTableInstr *) instrPtr;
    U32 labelIdx = tablePtr->labelIdx;
    Value i = rt.stack_.popValue();
    if (i.i32 < tablePtr->labelVec.size()) {
        labelIdx = tablePtr->labelVec[i.i32];
    }
    return op_break_impl(rt, labelIdx);
}

OP_FUNC_DEFINE(Op_return) {
    Frame *frame = &rt.stack_.frames.top();

    // pop value
    Value retVal;
    if (frame->retType != ValueTypeVoid) {
        retVal = rt.stack_.popValue();
    }

    // clear labels & values & current frame
    if (rt.stack_.labels.size() > frame->labelStackSize) {
        rt.stack_.popLabel(rt.stack_.labels.size() - frame->labelStackSize);
    }
    if (rt.stack_.values.size() > frame->valueStackSize) {
        rt.stack_.popValue(rt.stack_.values.size() - frame->valueStackSize);
    }

    rt.stack_.locals.sizeDecrease(frame->localSize);
    rt.stack_.popFrame();

    // push back value
    if (frame->retType != ValueTypeVoid) {
        rt.stack_.pushValue(retVal);
    }

    rt.pc_ = frame->pre;
    return true;
}

OP_FUNC_DEFINE(Op_call) {
    CtrlCallInstr *ptr = (CtrlCallInstr *) instrPtr;
    Frame *frame = &rt.stack_.frames.top();
    Ptr<FuncInstance> &funcInst = rt.store().funcs[frame->module->funcAddrs[ptr->idx]];
    return rt.invoke(*funcInst);
}

OP_FUNC_DEFINE(Op_call_indirect) {
    CtrlCallInstr *ptr = (CtrlCallInstr *) instrPtr;
    Frame *frame = &rt.stack_.frames.top();
    Addr &ta = frame->module->tableAddrs[0];
    Ptr<TableInstance> &tab = rt.store().tables[ta];

    Value i = rt.stack_.popValue();
    if (unlikely(i.i32 >= tab->elem.size())) {
        rt.trap_ = TrapUndefinedElement;
        return false;
    }

    if (unlikely(tab->elem[i.i32] >= rt.store().funcs.size())) {
        std::cout << "call_indirect func addr invalid\n";
        return false;
    }

    Ptr<FuncInstance> &funcInst = rt.store().funcs[tab->elem[i.i32]];
    if (unlikely(!frame->module->types[ptr->idx].equals(funcInst->type))) {
        rt.trap_ = TrapIndirectCallTypeMismatch;
        return false;
    }

    return rt.invoke(*funcInst);
}

OP_FUNC_DEFINE(Op_drop) {
    rt.stack_.popValue();
    return true;
}

OP_FUNC_DEFINE(Op_select) {
    Value c = rt.stack_.popValue();
    Value val = rt.stack_.popValue();
    if (c.i32 == 0) {
        rt.stack_.values.top() = val;
    }
    return true;
}

OP_FUNC_DEFINE(Op_get_local) {
    VariableInstr *ptr = (VariableInstr *) instrPtr;
    rt.stack_.pushValue(rt.stack_.frames.top().locals[ptr->idx]);
    return true;
}

OP_FUNC_DEFINE(Op_set_local) {
    VariableInstr *ptr = (VariableInstr *) instrPtr;
    rt.stack_.frames.top().locals[ptr->idx] = rt.stack_.popValue();
    return true;
}

OP_FUNC_DEFINE(Op_tee_local) {
    VariableInstr *ptr = (VariableInstr *) instrPtr;
    rt.stack_.frames.top().locals[ptr->idx] = rt.stack_.values.top();
    return true;
}

OP_FUNC_DEFINE(Op_get_global) {
    VariableInstr *ptr = (VariableInstr *) instrPtr;
    ModuleInstance *module = rt.stack_.frames.top().module;
    Ptr<GlobalInstance> &globalInst = rt.store().globals[module->globalAddrs[ptr->idx]];
    rt.stack_.pushValue(globalInst->value);
    return true;
}

OP_FUNC_DEFINE(Op_set_global) {
    VariableInstr *ptr = (VariableInstr *) instrPtr;
    ModuleInstance *module = rt.stack_.frames.top().module;
    Ptr<GlobalInstance> &globalInst = rt.store().globals[module->globalAddrs[ptr->idx]];
    globalInst->value = rt.stack_.popValue();
    return true;
}

void *op_load_impl(BaseInstr *instrPtr, Runtime &rt, U32 size) {
    MemoryInstr *ptr = (MemoryInstr *) instrPtr;
    ModuleInstance *module = rt.stack_.frames.top().module;
    Ptr<MemoryInstance> &memoryInst = rt.store().mems[module->memAddrs[0]];

    Value offset = rt.stack_.popValue();
    U64 ea = offset.i32 + (U64)ptr->offset;
    if (unlikely(offset.i32 < 0 || ea + size > memoryInst->dataSize)) {
        rt.trap_ = TrapOutOfBoundsMemoryAccess;
        return nullptr;
    }
    return &memoryInst->data[ea];
}

#define OP_TYPE_LOAD_IMPL(MType, VType)                     \
    void *ptr = op_load_impl(instrPtr, rt, sizeof(MType));  \
    if (likely(ptr != nullptr)) {                           \
        Value val(VType(*(MType *) ptr));                   \
        rt.stack_.pushValue(val);                           \
        return true;                                        \
    }                                                       \
    return false;                                           \


OP_FUNC_DEFINE(Op_i32_load) {
    OP_TYPE_LOAD_IMPL(I32,)
}

OP_FUNC_DEFINE(Op_i64_load) {
    OP_TYPE_LOAD_IMPL(I64,)
}

OP_FUNC_DEFINE(Op_f32_load) {
    OP_TYPE_LOAD_IMPL(F32,)
}

OP_FUNC_DEFINE(Op_f64_load) {
    OP_TYPE_LOAD_IMPL(F64,)
}

OP_FUNC_DEFINE(Op_i32_load8_s) {
    OP_TYPE_LOAD_IMPL(I8, Utils::signExt_8_32)
}

OP_FUNC_DEFINE(Op_i32_load8_u) {
    OP_TYPE_LOAD_IMPL(U8, I32)
}

OP_FUNC_DEFINE(Op_i32_load16_s) {
    OP_TYPE_LOAD_IMPL(I16, Utils::signExt_16_32)
}

OP_FUNC_DEFINE(Op_i32_load16_u) {
    OP_TYPE_LOAD_IMPL(U16, I32)
}

OP_FUNC_DEFINE(Op_i64_load8_s) {
    OP_TYPE_LOAD_IMPL(I8, Utils::signExt_8_64)
}

OP_FUNC_DEFINE(Op_i64_load8_u) {
    OP_TYPE_LOAD_IMPL(U8, I64)
}

OP_FUNC_DEFINE(Op_i64_load16_s) {
    OP_TYPE_LOAD_IMPL(I16, Utils::signExt_16_64)
}

OP_FUNC_DEFINE(Op_i64_load16_u) {
    OP_TYPE_LOAD_IMPL(U16, I64)
}

OP_FUNC_DEFINE(Op_i64_load32_s) {
    OP_TYPE_LOAD_IMPL(I32, Utils::signExt_32_64)
}

OP_FUNC_DEFINE(Op_i64_load32_u) {
    OP_TYPE_LOAD_IMPL(U32, I64)
}

void *op_store_impl(BaseInstr *instrPtr, Runtime &rt, U32 size, Value *val) {
    MemoryInstr *ptr = (MemoryInstr *) instrPtr;
    ModuleInstance *module = rt.stack_.frames.top().module;
    Ptr<MemoryInstance> &memoryInst = rt.store().mems[module->memAddrs[0]];

    *val = rt.stack_.popValue();

    Value offset = rt.stack_.popValue();
    U64 ea = offset.i32 + (U64)ptr->offset;
    if (unlikely(offset.i32 < 0 || ea + size > memoryInst->dataSize)) {
        rt.trap_ = TrapOutOfBoundsMemoryAccess;
        return nullptr;
    }
    return &memoryInst->data[ea];
}

#define OP_TYPE_STORE_IMPL(MType, VType)                            \
    Value val;                                                      \
    void *ptr = op_store_impl(instrPtr, rt, sizeof(MType), &val);   \
    if (likely(ptr != nullptr)) {                                   \
        *(MType *) ptr = (MType)val.VType;                          \
        return true;                                                \
    }                                                               \
    return false;                                                   \


OP_FUNC_DEFINE(Op_i32_store) {
    OP_TYPE_STORE_IMPL(U32, i32)
}

OP_FUNC_DEFINE(Op_i64_store) {
    OP_TYPE_STORE_IMPL(U64, i64)
}

OP_FUNC_DEFINE(Op_f32_store) {
    OP_TYPE_STORE_IMPL(F32, f32)
}

OP_FUNC_DEFINE(Op_f64_store) {
    OP_TYPE_STORE_IMPL(F64, f64)
}

OP_FUNC_DEFINE(Op_i32_store8) {
    OP_TYPE_STORE_IMPL(U8, i32)
}

OP_FUNC_DEFINE(Op_i32_store16) {
    OP_TYPE_STORE_IMPL(U16, i32)
}

OP_FUNC_DEFINE(Op_i64_store8) {
    OP_TYPE_STORE_IMPL(U8, i64)
}

OP_FUNC_DEFINE(Op_i64_store16) {
    OP_TYPE_STORE_IMPL(U16, i64)
}

OP_FUNC_DEFINE(Op_i64_store32) {
    OP_TYPE_STORE_IMPL(U32, i64)
}

OP_FUNC_DEFINE(Op_memory_size) {
    ModuleInstance *module = rt.stack_.frames.top().module;
    Ptr<MemoryInstance> &memoryInst = rt.store().mems[module->memAddrs[0]];
    Value sz(I32(memoryInst->pageCnt));
    rt.stack_.pushValue(sz);
    return true;
}

OP_FUNC_DEFINE(Op_memory_grow) {
    ModuleInstance *module = rt.stack_.frames.top().module;
    Ptr<MemoryInstance> &memoryInst = rt.store().mems[module->memAddrs[0]];
    U32 sz = memoryInst->pageCnt;

    Value n = rt.stack_.popValue();
    Value ret = memoryInst->grow(n.i32) ? Value((I32) sz) : Value((I32) -1);
    rt.stack_.pushValue(ret);
    return true;
}

#define OP_TYPE_CONST_IMPL                                      \
    NumericConstInstr *ptr = (NumericConstInstr *) instrPtr;    \
    rt.stack_.pushValue(ptr->val);                              \
    return true;                                                \


OP_FUNC_DEFINE(Op_i32_const) {
    OP_TYPE_CONST_IMPL
}

OP_FUNC_DEFINE(Op_i64_const) {
    OP_TYPE_CONST_IMPL
}

OP_FUNC_DEFINE(Op_f32_const) {
    OP_TYPE_CONST_IMPL
}

OP_FUNC_DEFINE(Op_f64_const) {
    OP_TYPE_CONST_IMPL
}

OP_FUNC_DEFINE(Op_i32_eqz) {
    Value val = rt.stack_.popValue();
    Value ret((I32) (val.i32 == 0 ? 1 : 0));
    rt.stack_.pushValue(ret);
    return true;
}

OP_FUNC_DEFINE(Op_i64_eqz) {
    Value val = rt.stack_.popValue();
    Value ret((I32) (val.i64 == 0 ? 1 : 0));
    rt.stack_.pushValue(ret);
    return true;
}

#define OP_TYPE_CMP_IMPL(SrcType, OpType, OP)                                   \
    Value val1 = rt.stack_.popValue();                                          \
    Value val2 = rt.stack_.popValue();                                          \
    Value ret((I32)(((SrcType)val2.OpType OP (SrcType)val1.OpType) ? 1 : 0));   \
    rt.stack_.pushValue(ret);                                                   \
    return true;                                                                \


OP_FUNC_DEFINE(Op_i32_eq) {
    OP_TYPE_CMP_IMPL(U32, i32, ==)
}

OP_FUNC_DEFINE(Op_i32_ne) {
    OP_TYPE_CMP_IMPL(U32, i32, !=)
}

OP_FUNC_DEFINE(Op_i32_lt_s) {
    OP_TYPE_CMP_IMPL(I32, i32, <)
}

OP_FUNC_DEFINE(Op_i32_lt_u) {
    OP_TYPE_CMP_IMPL(U32, i32, <)
}

OP_FUNC_DEFINE(Op_i32_gt_s) {
    OP_TYPE_CMP_IMPL(I32, i32, >)
}

OP_FUNC_DEFINE(Op_i32_gt_u) {
    OP_TYPE_CMP_IMPL(U32, i32, >)
}

OP_FUNC_DEFINE(Op_i32_le_s) {
    OP_TYPE_CMP_IMPL(I32, i32, <=)
}

OP_FUNC_DEFINE(Op_i32_le_u) {
    OP_TYPE_CMP_IMPL(U32, i32, <=)
}

OP_FUNC_DEFINE(Op_i32_ge_s) {
    OP_TYPE_CMP_IMPL(I32, i32, >=)
}

OP_FUNC_DEFINE(Op_i32_ge_u) {
    OP_TYPE_CMP_IMPL(U32, i32, >=)
}

OP_FUNC_DEFINE(Op_i64_eq) {
    OP_TYPE_CMP_IMPL(U64, i64, ==)
}

OP_FUNC_DEFINE(Op_i64_ne) {
    OP_TYPE_CMP_IMPL(U64, i64, !=)
}

OP_FUNC_DEFINE(Op_i64_lt_s) {
    OP_TYPE_CMP_IMPL(I64, i64, <)
}

OP_FUNC_DEFINE(Op_i64_lt_u) {
    OP_TYPE_CMP_IMPL(U64, i64, <)
}

OP_FUNC_DEFINE(Op_i64_gt_s) {
    OP_TYPE_CMP_IMPL(I64, i64, >)
}

OP_FUNC_DEFINE(Op_i64_gt_u) {
    OP_TYPE_CMP_IMPL(U64, i64, >)
}

OP_FUNC_DEFINE(Op_i64_le_s) {
    OP_TYPE_CMP_IMPL(I64, i64, <=)
}

OP_FUNC_DEFINE(Op_i64_le_u) {
    OP_TYPE_CMP_IMPL(U64, i64, <=)
}

OP_FUNC_DEFINE(Op_i64_ge_s) {
    OP_TYPE_CMP_IMPL(I64, i64, >=)
}

OP_FUNC_DEFINE(Op_i64_ge_u) {
    OP_TYPE_CMP_IMPL(U64, i64, >=)
}

OP_FUNC_DEFINE(Op_f32_eq) {
    OP_TYPE_CMP_IMPL(F32, f32, ==)
}

OP_FUNC_DEFINE(Op_f32_ne) {
    OP_TYPE_CMP_IMPL(F32, f32, !=)
}

OP_FUNC_DEFINE(Op_f32_lt) {
    OP_TYPE_CMP_IMPL(F32, f32, <)
}

OP_FUNC_DEFINE(Op_f32_gt) {
    OP_TYPE_CMP_IMPL(F32, f32, >)
}

OP_FUNC_DEFINE(Op_f32_le) {
    OP_TYPE_CMP_IMPL(F32, f32, <=)
}

OP_FUNC_DEFINE(Op_f32_ge) {
    OP_TYPE_CMP_IMPL(F32, f32, >=)
}

OP_FUNC_DEFINE(Op_f64_eq) {
    OP_TYPE_CMP_IMPL(F64, f64, ==)
}

OP_FUNC_DEFINE(Op_f64_ne) {
    OP_TYPE_CMP_IMPL(F64, f64, !=)
}

OP_FUNC_DEFINE(Op_f64_lt) {
    OP_TYPE_CMP_IMPL(F64, f64, <)
}

OP_FUNC_DEFINE(Op_f64_gt) {
    OP_TYPE_CMP_IMPL(F64, f64, >)
}

OP_FUNC_DEFINE(Op_f64_le) {
    OP_TYPE_CMP_IMPL(F64, f64, <=)
}

OP_FUNC_DEFINE(Op_f64_ge) {
    OP_TYPE_CMP_IMPL(F64, f64, >=)
}

#define OP_TYPE_BIT_IMPL(VType, RetType, OpType, OP)    \
    Value val = rt.stack_.popValue();                   \
    Value ret((RetType) OP((VType)val.OpType));         \
    rt.stack_.pushValue(ret);                           \
    return true;                                        \


OP_FUNC_DEFINE(Op_i32_clz) {
    OP_TYPE_BIT_IMPL(U32, I32, i32, Utils::CLZ)
}

OP_FUNC_DEFINE(Op_i32_ctz) {
    OP_TYPE_BIT_IMPL(U32, I32, i32, Utils::CTZ)
}

OP_FUNC_DEFINE(Op_i32_popcnt) {
    OP_TYPE_BIT_IMPL(U32, I32, i32, Utils::POPCNT)
}

OP_FUNC_DEFINE(Op_i64_clz) {
    OP_TYPE_BIT_IMPL(U64, I64, i64, Utils::CLZ)
}

OP_FUNC_DEFINE(Op_i64_ctz) {
    OP_TYPE_BIT_IMPL(U64, I64, i64, Utils::CTZ)
}

OP_FUNC_DEFINE(Op_i64_popcnt) {
    OP_TYPE_BIT_IMPL(U64, I64, i64, Utils::POPCNT)
}

#define OP_TYPE_NUM_IMPL(VType, RType, OpType, OP)                              \
    Value B = rt.stack_.popValue();                                             \
    Value &A = rt.stack_.values.top();                                          \
    A.VType = ((RType)A.VType) OP (OpType)B.VType;                              \
    return true;                                                                \


OP_FUNC_DEFINE(Op_i32_add) {
    OP_TYPE_NUM_IMPL(i32, U32, U32, +)
}

OP_FUNC_DEFINE(Op_i32_sub) {
    OP_TYPE_NUM_IMPL(i32, U32, U32, -)
}

OP_FUNC_DEFINE(Op_i32_mul) {
    OP_TYPE_NUM_IMPL(i32, U32, U32, *)
}

OP_FUNC_DEFINE(Op_i32_and) {
    OP_TYPE_NUM_IMPL(i32, U32, U32, &)
}

OP_FUNC_DEFINE(Op_i32_or) {
    OP_TYPE_NUM_IMPL(i32, U32, U32, |)
}

OP_FUNC_DEFINE(Op_i32_xor) {
    OP_TYPE_NUM_IMPL(i32, U32, U32, ^)
}

OP_FUNC_DEFINE(Op_i32_shl) {
    OP_TYPE_NUM_IMPL(i32, U32, U32, <<)
}

OP_FUNC_DEFINE(Op_i32_shr_s) {
    OP_TYPE_NUM_IMPL(i32, I32, U32, >>)
}

OP_FUNC_DEFINE(Op_i32_shr_u) {
    OP_TYPE_NUM_IMPL(i32, U32, U32, >>)
}

OP_FUNC_DEFINE(Op_i64_add) {
    OP_TYPE_NUM_IMPL(i64, U64, U64, +)
}

OP_FUNC_DEFINE(Op_i64_sub) {
    OP_TYPE_NUM_IMPL(i64, U64, U64, -)
}

OP_FUNC_DEFINE(Op_i64_mul) {
    OP_TYPE_NUM_IMPL(i64, U64, U64, *)
}

OP_FUNC_DEFINE(Op_i64_and) {
    OP_TYPE_NUM_IMPL(i64, U64, U64, &)
}

OP_FUNC_DEFINE(Op_i64_or) {
    OP_TYPE_NUM_IMPL(i64, U64, U64, |)
}

OP_FUNC_DEFINE(Op_i64_xor) {
    OP_TYPE_NUM_IMPL(i64, U64, U64, ^)
}

OP_FUNC_DEFINE(Op_i64_shl) {
    OP_TYPE_NUM_IMPL(i64, U64, U64, <<)
}

OP_FUNC_DEFINE(Op_i64_shr_s) {
    OP_TYPE_NUM_IMPL(i64, I64, I64, >>)
}

OP_FUNC_DEFINE(Op_i64_shr_u) {
    OP_TYPE_NUM_IMPL(i64, U64, U64, >>)
}

OP_FUNC_DEFINE(Op_f32_add) {
    OP_TYPE_NUM_IMPL(f32, F32, F32, +)
}

OP_FUNC_DEFINE(Op_f32_sub) {
    OP_TYPE_NUM_IMPL(f32, F32, F32, -)
}

OP_FUNC_DEFINE(Op_f32_mul) {
    OP_TYPE_NUM_IMPL(f32, F32, F32, *)
}

OP_FUNC_DEFINE(Op_f32_div) {
    OP_TYPE_NUM_IMPL(f32, F32, F32, /)
}

OP_FUNC_DEFINE(Op_f64_add) {
    OP_TYPE_NUM_IMPL(f64, F64, F64, +)
}

OP_FUNC_DEFINE(Op_f64_sub) {
    OP_TYPE_NUM_IMPL(f64, F64, F64, -)
}

OP_FUNC_DEFINE(Op_f64_mul) {
    OP_TYPE_NUM_IMPL(f64, F64, F64, *)
}

OP_FUNC_DEFINE(Op_f64_div) {
    OP_TYPE_NUM_IMPL(f64, F64, F64, /)
}

#define OP_TYPE_ROT_IMPL(VType, OP)                         \
    Value val = rt.stack_.popValue();                       \
    Value ret(OP(rt.stack_.values.top().VType, val.VType)); \
    rt.stack_.values.setTop(ret);                           \
    return true;                                            \


OP_FUNC_DEFINE(Op_i32_rotl) {
    OP_TYPE_ROT_IMPL(i32, (I32) Utils::ROTL_32)
}

OP_FUNC_DEFINE(Op_i32_rotr) {
    OP_TYPE_ROT_IMPL(i32, (I32) Utils::ROTR_32)
}

OP_FUNC_DEFINE(Op_i64_rotl) {
    OP_TYPE_ROT_IMPL(i64, (I64) Utils::ROTL_64)
}

OP_FUNC_DEFINE(Op_i64_rotr) {
    OP_TYPE_ROT_IMPL(i64, (I64) Utils::ROTR_64)
}

#define OP_TYPE_DIV_REM_IMPL(VType, RType, OpType, OP)                              \
    Value val = rt.stack_.popValue();                                               \
    if (unlikely(val.VType == 0)) {                                                 \
        rt.trap_ = TrapIntegerDivideByZero;                                         \
        return false;                                                               \
    }                                                                               \
    Value ret(OpType(((RType)rt.stack_.values.top().VType) OP ((RType)val.VType))); \
    rt.stack_.values.setTop(ret);                                                   \
    return true;                                                                    \


OP_FUNC_DEFINE(Op_i32_div_s) {
    Value &A = rt.stack_.values.top(1);
    Value &B = rt.stack_.values.top(0);
    if (unlikely(B.i32 == -1) && unlikely(A.i32 == INT32_MIN)) {
        rt.trap_ = TrapIntegerOverflow;
        return false;
    }
    OP_TYPE_DIV_REM_IMPL(i32, I32, I32, /)
}

OP_FUNC_DEFINE(Op_i32_div_u) {
    OP_TYPE_DIV_REM_IMPL(i32, U32, I32, /)
}

OP_FUNC_DEFINE(Op_i32_rem_s) {
    Value &A = rt.stack_.values.top(1);
    Value &B = rt.stack_.values.top(0);
    if (unlikely(B.i32 == -1) && unlikely(A.i32 == INT32_MIN)) {
        rt.stack_.popValue();
        A.i32 = 0;
        return true;
    }
    OP_TYPE_DIV_REM_IMPL(i32, I32, I32, %)
}

OP_FUNC_DEFINE(Op_i32_rem_u) {
    OP_TYPE_DIV_REM_IMPL(i32, U32, I32, %)
}

OP_FUNC_DEFINE(Op_i64_div_s) {
    Value &A = rt.stack_.values.top(1);
    Value &B = rt.stack_.values.top(0);
    if (unlikely(B.i64 == -1) && unlikely(A.i64 == INT64_MIN)) {
        rt.trap_ = TrapIntegerOverflow;
        return false;
    }
    OP_TYPE_DIV_REM_IMPL(i64, I64, I64, /)
}

OP_FUNC_DEFINE(Op_i64_div_u) {
    OP_TYPE_DIV_REM_IMPL(i64, U64, I64, /)
}

OP_FUNC_DEFINE(Op_i64_rem_s) {
    Value &A = rt.stack_.values.top(1);
    Value &B = rt.stack_.values.top(0);
    if (unlikely(B.i64 == -1) && unlikely(A.i64 == INT64_MIN)) {
        rt.stack_.popValue();
        A.i64 = 0;
        return true;
    }
    OP_TYPE_DIV_REM_IMPL(i64, I64, I64, %)
}

OP_FUNC_DEFINE(Op_i64_rem_u) {
    OP_TYPE_DIV_REM_IMPL(i64, U64, I64, %)
}

#define OP_TYPE_MATH_ONE_IMPL(VType, OpType, OP)            \
    Value ret(OpType(OP(rt.stack_.values.top().VType)));    \
    rt.stack_.values.setTop(ret);                           \
    return true;                                            \


OP_FUNC_DEFINE(Op_f32_abs) {
    OP_TYPE_MATH_ONE_IMPL(f32, F32, std::fabs)
}

OP_FUNC_DEFINE(Op_f32_neg) {
    OP_TYPE_MATH_ONE_IMPL(f32, F32, -)
}

OP_FUNC_DEFINE(Op_f32_ceil) {
    OP_TYPE_MATH_ONE_IMPL(f32, F32, std::ceil)
}

OP_FUNC_DEFINE(Op_f32_floor) {
    OP_TYPE_MATH_ONE_IMPL(f32, F32, std::floor)
}

OP_FUNC_DEFINE(Op_f32_trunc) {
    OP_TYPE_MATH_ONE_IMPL(f32, F32, std::truncf)
}

OP_FUNC_DEFINE(Op_f32_nearest) {
    OP_TYPE_MATH_ONE_IMPL(f32, F32, std::rintf)
}

OP_FUNC_DEFINE(Op_f32_sqrt) {
    OP_TYPE_MATH_ONE_IMPL(f32, F32, std::sqrt)
}

OP_FUNC_DEFINE(Op_f64_abs) {
    OP_TYPE_MATH_ONE_IMPL(f64, F64, std::fabs)
}

OP_FUNC_DEFINE(Op_f64_neg) {
    OP_TYPE_MATH_ONE_IMPL(f64, F64, -)
}

OP_FUNC_DEFINE(Op_f64_ceil) {
    OP_TYPE_MATH_ONE_IMPL(f64, F64, std::ceil)
}

OP_FUNC_DEFINE(Op_f64_floor) {
    OP_TYPE_MATH_ONE_IMPL(f64, F64, std::floor)
}

OP_FUNC_DEFINE(Op_f64_trunc) {
    OP_TYPE_MATH_ONE_IMPL(f64, F64, std::trunc)
}

OP_FUNC_DEFINE(Op_f64_nearest) {
    OP_TYPE_MATH_ONE_IMPL(f64, F64, std::rint)
}

OP_FUNC_DEFINE(Op_f64_sqrt) {
    OP_TYPE_MATH_ONE_IMPL(f64, F64, std::sqrt)
}

#define OP_TYPE_MATH_TWO_IMPL(VType, OpType, OP)                    \
    Value val = rt.stack_.popValue();                               \
    Value ret(OpType(OP(rt.stack_.values.top().VType, val.VType))); \
    rt.stack_.values.setTop(ret);                                   \
    return true;                                                    \


#define OP_FLOAT_MIN_MAX_IMPL(VType, OpType, OP)        \
    Value &a = rt.stack_.values.top(1);                 \
    Value &b = rt.stack_.values.top(0);                 \
    a.VType = Utils::OP##_##OpType(a.VType, b.VType);   \
    rt.stack_.popValue();                               \
    return true;                                        \

OP_FUNC_DEFINE(Op_f32_min) {
    OP_FLOAT_MIN_MAX_IMPL(f32, F32, MIN)
}

OP_FUNC_DEFINE(Op_f32_max) {
    OP_FLOAT_MIN_MAX_IMPL(f32, F32, MAX)
}

OP_FUNC_DEFINE(Op_f32_copysign) {
    OP_TYPE_MATH_TWO_IMPL(f32, F32, std::copysignf)
}

OP_FUNC_DEFINE(Op_f64_min) {
    OP_FLOAT_MIN_MAX_IMPL(f64, F64, MIN)
}

OP_FUNC_DEFINE(Op_f64_max) {
    OP_FLOAT_MIN_MAX_IMPL(f64, F64, MAX)
}

OP_FUNC_DEFINE(Op_f64_copysign) {
    OP_TYPE_MATH_TWO_IMPL(f64, F64, std::copysign)
}

OP_FUNC_DEFINE(Op_i32_wrap_i64) {
    Value ret(I32(((I32)(rt.stack_.values.top().i64)) &0xFFFFFFFFLL));
    rt.stack_.values.setTop(ret);
    return true;
}

#define OP_TYPE_TRUNC_IMPL(VType, RType, OpType, Min, Max)  \
    Value &v = rt.stack_.values.top();                      \
    if (unlikely(std::isnan(v.VType))) {                    \
        rt.trap_ = TrapInvalidConversionToInteger;          \
        return false;                                       \
    }                                                       \
    if (unlikely(v.VType <= Min || v.VType >= Max)) {       \
        rt.trap_ = TrapIntegerOverflow;                     \
        return false;                                       \
    }                                                       \
    v.OpType = (RType)v.VType;                              \
    return true;                                            \


OP_FUNC_DEFINE(Op_i32_trunc_s_f32) {
    OP_TYPE_TRUNC_IMPL(f32, I32, i32, -2147483904.0f, 2147483648.0f)
}

OP_FUNC_DEFINE(Op_i32_trunc_u_f32) {
    OP_TYPE_TRUNC_IMPL(f32, U32, i32, -1.0f, 4294967296.0f)
}

OP_FUNC_DEFINE(Op_i32_trunc_s_f64) {
    OP_TYPE_TRUNC_IMPL(f64, I32, i32, -2147483649.0, 2147483648.0)
}

OP_FUNC_DEFINE(Op_i32_trunc_u_f64) {
    OP_TYPE_TRUNC_IMPL(f64, U32, i32, -1.0, 4294967296.0)
}

OP_FUNC_DEFINE(Op_i64_trunc_s_f32) {
    OP_TYPE_TRUNC_IMPL(f32, I64, i64, -9223373136366403584.0f,  9223372036854775808.0f)
}

OP_FUNC_DEFINE(Op_i64_trunc_u_f32) {
    OP_TYPE_TRUNC_IMPL(f32, U64, i64, -1.0f, 18446744073709551616.0f)
}

OP_FUNC_DEFINE(Op_i64_trunc_s_f64) {
    OP_TYPE_TRUNC_IMPL(f64, I64, i64, -9223372036854777856.0,  9223372036854775808.0)
}

OP_FUNC_DEFINE(Op_i64_trunc_u_f64) {
    OP_TYPE_TRUNC_IMPL(f64, U64, i64, -1.0, 18446744073709551616.0)
}

#define OP_TYPE_CVT_IMPL(VType, RType, OpType)          \
    Value &v = rt.stack_.values.top();                  \
    v.OpType = (RType)v.VType;                          \
    return true;                                        \


OP_FUNC_DEFINE(Op_i64_extend_s_i32) {
    OP_TYPE_CVT_IMPL(i32, I32, i64)
}

OP_FUNC_DEFINE(Op_i64_extend_u_i32) {
    OP_TYPE_CVT_IMPL(i32, U32, i64)
}

OP_FUNC_DEFINE(Op_f32_convert_s_i32) {
    OP_TYPE_CVT_IMPL(i32, I32, f32)
}

OP_FUNC_DEFINE(Op_f32_convert_u_i32) {
    OP_TYPE_CVT_IMPL(i32, U32, f32)
}

OP_FUNC_DEFINE(Op_f32_convert_s_i64) {
    OP_TYPE_CVT_IMPL(i64, I64, f32)
}

OP_FUNC_DEFINE(Op_f32_convert_u_i64) {
    OP_TYPE_CVT_IMPL(i64, U64, f32)
}

OP_FUNC_DEFINE(Op_f32_demote_f64) {
    OP_TYPE_CVT_IMPL(f64, F32, f32)
}

OP_FUNC_DEFINE(Op_f64_promote_f32) {
    OP_TYPE_CVT_IMPL(f32, F64, f64)
}

OP_FUNC_DEFINE(Op_f64_convert_s_i32) {
    OP_TYPE_CVT_IMPL(i32, I32, f64)
}

OP_FUNC_DEFINE(Op_f64_convert_u_i32) {
    OP_TYPE_CVT_IMPL(i32, U32, f64)
}

OP_FUNC_DEFINE(Op_f64_convert_s_i64) {
    OP_TYPE_CVT_IMPL(i64, I64, f64)
}

OP_FUNC_DEFINE(Op_f64_convert_u_i64) {
    OP_TYPE_CVT_IMPL(i64, U64, f64)
}

OP_FUNC_DEFINE(Op_i32_reinterpret_f32) {
    return true;
}

OP_FUNC_DEFINE(Op_i64_reinterpret_f64) {
    return true;
}

OP_FUNC_DEFINE(Op_f32_reinterpret_i32) {
    return true;
}

OP_FUNC_DEFINE(Op_f64_reinterpret_i64) {
    return true;
}

}
