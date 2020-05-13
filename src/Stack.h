//
// Created by Keith on 2019/12/27.
//

#ifndef TWA_STACK_H
#define TWA_STACK_H


#include <iostream>
#include "Store.h"

namespace TWA {

struct ProgramCounter {
    ProgramCounter() : instrPtr(nullptr), instrSize(0), instrIdx(0) {}

    inline bool dummy() { return instrPtr == nullptr; }
    inline bool end() { return instrIdx >= instrSize; }

    inline BaseInstr *nextInstr() { return instrPtr[instrIdx++]; }
    inline ProgramCounter &forward() { instrIdx++; return *this; }
    inline ProgramCounter &backward() { instrIdx--; return *this; }

    inline void set(ExprCodes *ins, U32 idx) {
        instrPtr = ins->codes;
        instrSize = ins->size;
        instrIdx = idx;
    }

    inline void set(BaseInstr **ip, U32 size,  U32 idx) {
        instrPtr = ip;
        instrSize = size;
        instrIdx = idx;
    }

    U32 instrSize = 0;
    U32 instrIdx = 0;
    BaseInstr **instrPtr = nullptr;
};

struct Label {
    inline void reset(ValueType rt, ProgramCounter &p, ProgramCounter &n, U32 vs) {
        retType = rt;
        pre = p;
        next = n;
        valueStackSize = vs;
    }

    ProgramCounter pre;
    ProgramCounter next;
    U32 valueStackSize = 0;
    ValueType retType;
};

struct FrameBlockInstr {
    inline void reset(FuncInstance &funcInst) {
        blockInstr.resultType = funcInst.type.retType;
        blockInstr.expr = funcInst.codes;
        blockInstr.opCode = Op_block;
        endInstr.opCode = Op_end;
    }

    CtrlBlockInstr blockInstr;
    BaseInstr endInstr;

    BaseInstr* ins[2] = { &blockInstr, &endInstr };
};

struct Frame {
    inline void reset(ProgramCounter &p, FuncInstance &funcInst) {
        pre = p;
        fbi.reset(funcInst);
        module = funcInst.module;
        retType = funcInst.type.retType;
    }

    inline void setDummy() {
        fbi.ins[0] = nullptr;
    }

    ModuleInstance *module = nullptr;
    ProgramCounter pre;

    U32 valueStackSize = 0;
    U32 labelStackSize = 0;

    FrameBlockInstr fbi;

    Value *locals = nullptr;
    U32 localSize = 0;

    ValueType retType;
};

template<typename T>
class StackInternal {
public:
    inline void create(U32 stackSize) {
        capacity_ = stackSize;
        m_ = new T[capacity_];
        pos_ = 0;
    }

    inline void reset(bool clearMem = false) {
        pos_ = 0;
    }

    inline void release() {
        delete[] m_;
        m_ = nullptr;
        pos_ = 0;
        capacity_ = 0;
    }

    inline bool empty() { return 0 == pos_; }
    inline bool full(U32 append = 0) { return pos_ + append >= capacity_; }
    inline void pop() { pos_--; }
    inline void push(T &t) { m_[pos_] = t; pos_++; }
    inline T &top(U32 idx = 0) { return m_[pos_ - 1 - idx]; }
    inline void setTop(T &t, U32 idx = 0) { m_[pos_ - 1 - idx] = t; }
    inline U32 size() const { return pos_; }
    inline void sizeIncrease(U32 size) { pos_ += size; }
    inline void sizeDecrease(U32 size) { pos_ -= size; }
    inline T* data() { return m_; }

private:
    T *m_ = nullptr;
    U32 pos_ = 0;
    U32 capacity_ = 0;
};


struct Stack {
    inline void create(U32 stackSize) {
        values.create(stackSize / sizeof(Value));
        labels.create(stackSize / sizeof(Label));
        frames.create(stackSize / sizeof(Frame));
        locals.create(MAX_FUNC_LOCAL_SIZE);
    }

    inline void reset() {
        values.reset();
        labels.reset();
        frames.reset();
        locals.reset();
    }

    inline void release() {
        values.release();
        labels.release();
        frames.release();
        locals.release();
    }

    inline void pushValue(Value &val) {
        values.push(val);
    }

    inline void pushLabel(ValueType rt, ProgramCounter &p, ProgramCounter &n) {
        labels.sizeIncrease(1);
        labels.top().reset(rt, p, n, values.size());
    }

    inline void pushFrame(ProgramCounter &p, FuncInstance &funcInst) {
        frames.sizeIncrease(1);
        Frame *frame = &frames.top();
        frame->reset(p, funcInst);
        frame->valueStackSize = values.size();
        frame->labelStackSize = labels.size();
    }

    inline void pushDummyFrame() {
        frames.sizeIncrease(1);
        frames.top().setDummy();
    }

    inline Value popValue() {
        Value val = values.top();
        values.pop();
        return val;
    }

    inline void popValue(U32 n) {
        for (U32 i = 0; i < n; i++) {
            popValue();
        }
    }

    inline void popLabel() {
        labels.sizeDecrease(1);
    }

    inline void popLabel(U32 n) {
        for (U32 i = 0; i < n; i++) {
            popLabel();
        }
    }

    inline void popFrame() {
        frames.sizeDecrease(1);
    }

    StackInternal<Value>  values;
    StackInternal<Label>  labels;
    StackInternal<Frame>  frames;
    StackInternal<Value>  locals;
};

}
#endif //TWA_STACK_H
