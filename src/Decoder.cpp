//
// Created by Keith on 2019/12/18.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include "Decoder.h"
#include "Module.h"
#include "OpCode.h"

namespace TWA {

Decoder::Decoder(Module &module) : _file(nullptr),
                                   _module(module) {
    secFunctions[SectionCustom]     = &Decoder::readSection_custom;
    secFunctions[SectionType]       = &Decoder::readSection_type;
    secFunctions[SectionImport]     = &Decoder::readSection_import;
    secFunctions[SectionFunction]   = &Decoder::readSection_function;
    secFunctions[SectionTable]      = &Decoder::readSection_table;
    secFunctions[SectionMemory]     = &Decoder::readSection_memory;
    secFunctions[SectionGlobal]     = &Decoder::readSection_global;
    secFunctions[SectionExport]     = &Decoder::readSection_export;
    secFunctions[SectionStart]      = &Decoder::readSection_start;
    secFunctions[SectionElement]    = &Decoder::readSection_element;
    secFunctions[SectionCode]       = &Decoder::readSection_code;
    secFunctions[SectionData]       = &Decoder::readSection_data;
}

bool Decoder::decodeStream(std::istream &stream) {
    _file = &stream;
    bool decodeOk = readMagic();
    if (decodeOk) {
        decodeOk = readVersion();
        bool finished = false;
        while (!finished) {
            SectionState state = readSection();
            finished = (state != SectionStateSuccess);
            decodeOk = (state != SectionStateFail);
        }
    }

    return decodeOk;
}

bool Decoder::decodeFile(std::string &filePath) {
    bool decodeOk = false;
    std::ifstream ifs(filePath, std::ios::in | std::ios::binary);
    if (ifs.is_open()) {
        decodeOk = decodeStream(ifs);
        ifs.close();
    }

    return decodeOk;
}

bool Decoder::decodeBytes(const U8 *bytes) {
    bool decodeOk = false;
    std::istringstream iss((char*)bytes, std::ios::in | std::ios::binary);
    decodeOk = decodeStream(iss);

    return decodeOk;
}

bool Decoder::readMagic() {
    U32 magic = readElement<U32>();
    return WasmMagic == magic;
}

bool Decoder::readVersion() {
    _module.version = readElement<U32>();
    return WasmVersion == _module.version;
}

SectionState Decoder::readSection() {
    U8 secId = readElement<U8>();
    if (_file->eof()) {
        return SectionStateEOF;
    }

    if (secId >= SectionLength) {
        std::cout << "section invalid: " << secId << "\n";
        return SectionStateFail;
    }

    U32 secSize = readLebU32();

    bool decodeOk = (this->*secFunctions[secId])(secSize);
    return decodeOk ? SectionStateSuccess : SectionStateFail;
}

bool Decoder::readSection_custom(U32 len) {
    U32 pos = _file->tellg();
    std::string name = readName();
    if (name == CUSTOM_SECTION_NAME) {
        while (_file->tellg() < pos + len) {
            U8 secId = readElement<U8>();
            U32 secSize = readLebU32();
            switch (secId) {
                case NameSectionModule: {
                    _module.name = readName();
                    break;
                }
                case NameSectionFunction:
                case NameSectionLocal:
                default: {
                    std::cout << "skip name section: " << char('0' + secId) << "\n";
                    _file->seekg(secSize, std::ios::cur);
                    break;
                }
            }
        }
    } else {
        std::cout << "skip custom section, name: " << name << "\n";
        _file->seekg(pos + len, std::ios::beg);
    }
    return true;
}

bool Decoder::readSection_type(U32 len) {
    U32 vecSize = readLebU32();
    for (U32 i = 0; i < vecSize; i++) {
        FuncType funcType = readFuncType();
        _module.types.push_back(funcType);
    }

    return true;
}

bool Decoder::readSection_import(U32 len) {
    U32 vecSize = readLebU32();
    for (U32 i = 0; i < vecSize; i++) {
        SegImport imp;
        imp.module = readName();
        imp.name = readName();
        imp.desc.type = readElement<ExtType>();
        switch (imp.desc.type) {
            case ExtTypeFunc:      imp.desc.typeIdx      = readLebU32();         break;
            case ExtTypeTable:     imp.desc.tableType    = readTableType();      break;
            case ExtTypeMem:       imp.desc.memType      = readMemType();        break;
            case ExtTypeGlobal:    imp.desc.globalType   = readGlobalType();     break;
            default:
                std::cout << "import type invalid: " << imp.desc.type << "\n";   break;
        }
        _module.imports.push_back(imp);
    }
    return true;
}

bool Decoder::readSection_function(U32 len) {
    U32 vecSize = readLebU32();
    for (U32 i = 0; i < vecSize; i++) {
        SegFunc func;
        func.typeIdx = readLebU32();
        _module.funcs.push_back(func);
    }
    return true;
}

bool Decoder::readSection_table(U32 len) {
    U32 vecSize = readLebU32();
    for (U32 i = 0; i < vecSize; i++) {
        SegTable table;
        table.type = readTableType();
        _module.tables.push_back(table);
    }
    return true;
}

bool Decoder::readSection_memory(U32 len) {
    U32 vecSize = readLebU32();
    for (U32 i = 0; i < vecSize; i++) {
        SegMemory memory;
        memory.type = readMemType();
        _module.mems.push_back(memory);
    }
    return true;
}

bool Decoder::readSection_global(U32 len) {
    U32 vecSize = readLebU32();
    for (U32 i = 0; i < vecSize; i++) {
        SegGlobal global;
        global.type = readGlobalType();
        readExpr(global.init, 0);
        _module.globals.push_back(global);
    }
    return true;
}

bool Decoder::readSection_export(U32 len) {
    U32 vecSize = readLebU32();
    for (U32 i = 0; i < vecSize; i++) {
        SegExport exp;
        exp.name = readName();
        exp.desc.type = readElement<ExtType>();
        exp.desc.addr = readLebU32();
        _module.exports.push_back(exp);
    }
    return true;
}

bool Decoder::readSection_start(U32 len) {
    _module.start.valid = true;
    _module.start.funcIdx = readLebU32();
    return true;
}

bool Decoder::readSection_element(U32 len) {
    U32 vecSize = readLebU32();
    for (U32 i = 0; i < vecSize; i++) {
        SegElem elem;
        elem.tableIdx = readLebU32();
        readExpr(elem.offset, 0);
        U32 idxSize = readLebU32();
        for (int j = 0; j < idxSize; j++) {
            elem.init.push_back(readLebU32());
        }
        _module.elems.push_back(elem);
    }
    return true;
}

bool Decoder::readSection_code(U32 len) {
    U32 vecSize = readLebU32();
    if (vecSize > _module.funcs.size()) {
        std::cout << "code size invalid: " << vecSize << "\n";
        return false;
    }

    for (U32 i = 0; i < vecSize; i++) {
        SegFunc &func = _module.funcs[i];

        // size
        U32 size = readLebU32();

        // locals
        U32 localSize = readLebU32();
        func.localCnt = 0;
        for (U32 j = 0; j < localSize; j++) {
            func.localCnt += readLebU32();
            readElement<ValueType>();
        }

        // expr
        readExpr(func.codes, size);
    }
    return true;
}

bool Decoder::readSection_data(U32 len) {
    U32 vecSize = readLebU32();
    for (U32 i = 0; i < vecSize; i++) {
        SegData data;
        data.memIdx = readLebU32();
        readExpr(data.offset, 0);
        U32 byteSize = readLebU32();
        for (int j = 0; j < byteSize; j++) {
            data.init.push_back(readElement<U8>());
        }
        _module.datas.push_back(data);
    }
    return true;
}

U32 Decoder::readLebU32() {
    return readLebU64();
}

I32 Decoder::readLebI32() {
    return readLebI64();
}

U64 Decoder::readLebU64() {
    U64 ret = 0;
    U8 byte = 0;
    U8 shift = 0;
    do {
        byte = readElement<U8>();
        U64 slice = byte & 0x7f;
        if (shift >= 64 || slice << shift >> shift != slice) {
            std::cout << "uleb128 too big for U64\n";
            return 0;
        }
        ret += U64(byte & 0x7f) << shift;
        shift += 7;
    } while (byte >= 128);
    return ret;
}

I64 Decoder::readLebI64() {
    I64 ret = 0;
    U8 byte = 0;
    U8 shift = 0;
    do {
        byte = readElement<U8>();
        ret |= (uint64_t(byte & 0x7f) << shift);
        shift += 7;
    } while (byte >= 128);

    if (shift < 64 && (byte & 0x40)) {
        ret |= (-1ULL) << shift;
    }
    return ret;
}

template<typename T>
T Decoder::readElement() {
    T ret;
    char *p = (char *) &ret;
    _file->read(p, sizeof(T));
    return ret;
}

void Decoder::readFuncSignature(FuncSignature &sig) {
    sig.size = readLebU32();
    for (U32 i = 0; i < sig.size; i++) {
        sig.types[i] = readElement<ValueType>();
    }
}

std::string Decoder::readName() {
    U32 len = readLebU32();
    std::string str;
    if (len > 0) {
        char *p = new char[len + 1];
        memset(p, 0, len + 1);
        _file->read(p, len);
        str = std::string(p);
        delete[] p;
    }

    return str;
}

bool Decoder::readBoolValue() {
    U8 flag = readElement<U8>();
    return (flag == WasmTrue);
}

FuncType Decoder::readFuncType() {
    FuncType ret;
    U8 type = readElement<U8>();
    if (WasmValueTypeFunc != type) {
        std::cout << "funcType invalid: " << type << "\n";
    }
    // parameter types & result types
    readFuncSignature(ret.params);

    FuncSignature fs;
    readFuncSignature(fs);
    ret.retType = (fs.size <= 0 ? ValueTypeVoid : fs.types[0]);
    return ret;
}

Limit Decoder::readLimit() {
    Limit ret;
    bool hasMax = readBoolValue();
    ret.min = readLebU32();
    if (hasMax) {
        ret.max = readLebU32();
    } else {
        ret.max = 0;
    }
    return ret;
}

MemType Decoder::readMemType() {
    return readLimit();
}

TableType Decoder::readTableType() {
    TableType ret;
    ret.elemType = readElement<ValueType>();
    if (WasmValueTypeFuncRef != ret.elemType) {
        std::cout << "elemType invalid: " << ret.elemType << "\n";
    }
    ret.limit = readLimit();
    return ret;
}

GlobalType Decoder::readGlobalType() {
    GlobalType ret;
    ret.valueType = readElement<ValueType>();
    ret.mut = readBoolValue();
    return ret;
}

void Decoder::readExpr(ExprCodes &expr, U32 size) {
    std::vector<BaseInstr*> instrList;
    U8 opCode = readElement<U8>();
    while (opCode != Op_end) {
        BaseInstr *ins = nullptr;
        if (opCode <= Op_call_indirect) {
            readControlInstr(opCode, &ins);
        } else if (opCode <= Op_select) {
            ins = new ParametricInstr;
        } else if (opCode <= Op_set_global) {
            ins = new VariableInstr;
            ((VariableInstr *) ins)->idx = readLebU32();
        } else if (opCode <= Op_i64_store32) {
            ins = new MemoryInstr;
            ((MemoryInstr *) ins)->align = readLebU32();
            ((MemoryInstr *) ins)->offset = readLebU32();
        } else if (opCode <= Op_memory_grow) {
            ins = new MemoryInstr;
            U8 flag = readElement<U8>();
            if (flag != WasmZeroByte) {
                std::cout << "flag invalid: " << flag << "\n";
                break;
            }
        } else if (opCode <= Op_f64_const) {
            ins = new NumericConstInstr;
            readNumericConstInstr(opCode, (NumericConstInstr *) ins);
        } else if (opCode <= Op_f64_reinterpret_i64) {
            ins = new NumericInstr;
        } else {
            std::cout << "opCode invalid: " << opCode << "\n";
            break;
        }

        if (ins != nullptr) {
            ins->opCode = opCode;
            instrList.push_back(ins);
        }
        opCode = readElement<U8>();
    }

    // append end
    instrList.push_back(new BaseInstr(Op_end));

    // instrList -> expr
    expr.size = instrList.size();
    expr.codes = new BaseInstr*[expr.size];
    for (U32 idx = 0; idx < expr.size; idx++) {
        expr.codes[idx] = instrList[idx];
    }
}

void Decoder::readControlInstr(U8 opCode, BaseInstr **ins) {
    switch (opCode) {
        case Op_unreachable:
        case Op_nop:
        case Op_else:
        case Op_return: {
            *ins = new CtrlBaseInstr;
            break;
        }
        case Op_block:
        case Op_loop:
        case Op_if: {
            CtrlBlockInstr *cbPtr = new CtrlBlockInstr;
            *ins = cbPtr;
            cbPtr->resultType = readElement<ValueType>();
            readExpr(cbPtr->expr, 0);

            //  if else
            if (opCode == Op_if) {
                U32 idx = 0;
                for (; idx < cbPtr->expr.size; idx++) {
                    if (cbPtr->expr.codes[idx]->opCode == Op_else) {
                        break;
                    }
                }
                cbPtr->elseIdx = idx;

                // replace Op_else with Op_end
                if (cbPtr->expr.size > 0 && cbPtr->elseIdx < cbPtr->expr.size) {
                    cbPtr->expr.codes[cbPtr->elseIdx] = new BaseInstr(Op_end);
                }
            }
            break;
        }
        case Op_br:
        case Op_br_if: {
            *ins = new CtrlBrInstr;
            ((CtrlBrInstr *) (*ins))->labelIdx = readLebU32();
            break;
        }
        case Op_br_table: {
            *ins = new CtrlBrTableInstr;
            U32 vecSize = readLebU32();
            for (U32 i = 0; i < vecSize; i++) {
                U32 idx = readLebU32();
                ((CtrlBrTableInstr *) (*ins))->labelVec.push_back(idx);
            }
            ((CtrlBrTableInstr *) (*ins))->labelIdx = readLebU32();
            break;
        }
        case Op_call:
        case Op_call_indirect: {
            *ins = new CtrlCallInstr;
            ((CtrlCallInstr *) (*ins))->idx = readLebU32();
            if (Op_call_indirect == opCode) {
                U8 flag = readElement<U8>();
                if (WasmZeroByte != flag) {
                    std::cout << "Op_call_indirect flag invalid: " << flag << "\n";
                }
            }
            break;
        }
        default: {
            std::cout << "Control opCode invalid: " << opCode << "\n";
            break;
        }
    }
}

void Decoder::readNumericConstInstr(U8 opCode, NumericConstInstr* ins) {
    switch (opCode) {
        case Op_i32_const: ins->val = Value(readLebI32());                 break;
        case Op_i64_const: ins->val = Value(readLebI64());                 break;
        case Op_f32_const: ins->val = Value(readElement<F32>());           break;
        case Op_f64_const: ins->val = Value(readElement<F64>());           break;
        default:
            std::cout << "Const numeric opCode invalid: " << opCode << "\n";   break;
    }
}

}