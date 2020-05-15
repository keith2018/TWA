//
// Created by Keith on 2019/12/25.
//

#include <map>
#include <iostream>
#include "instantiation.h"
#include "runtime.h"
#include "opcode.h"


namespace TWA {

bool Instantiation::instantiate(Ptr<ModuleInstance> &moduleInst,
                                Runtime &runtime,
                                Module &module,
                                std::vector<ExternalValAddr> &extVal) {
    // imports check
    if (module.imports.size() != extVal.size()) {
        std::cout << "module.imports.size() != extVal.size()\n";
        return false;
    }

    for (U32 i = 0; i < module.imports.size(); i++) {
        if (module.imports[i].desc.type != extVal[i].type) {
            std::cout << "extVal.type not equal to imports.type: " << i << "\n";
            return false;
        }
    }

    // allocation
    moduleInst->types = module.types;

    // extract imports
    for (auto &ev : extVal) {
        switch (ev.type) {
            case ExtTypeFunc:   moduleInst->funcAddrs.push_back(ev.addr);       break;
            case ExtTypeTable:  moduleInst->tableAddrs.push_back(ev.addr);      break;
            case ExtTypeMem:    moduleInst->memAddrs.push_back(ev.addr);        break;
            case ExtTypeGlobal: moduleInst->globalAddrs.push_back(ev.addr);     break;
            default: std::cout << "extVal.type not valid " << ev.type << "\n";  break;
        }
    }

    // funcs
    for (auto &sf : module.funcs) {
        runtime.store().funcs.push_back(Ptr<FuncInstance>(new WasmFuncInstance(module.types, sf, moduleInst.get())));
        moduleInst->funcAddrs.emplace_back(runtime.store().funcs.size() - 1);
    }

    // table
    for (auto &st : module.tables) {
        runtime.store().tables.push_back(Ptr<TableInstance>(new TableInstance(st.type)));
        moduleInst->tableAddrs.emplace_back(runtime.store().tables.size() - 1);
    }

    // memory
    for (auto &sm : module.mems) {
        runtime.store().mems.push_back(Ptr<MemoryInstance>(new MemoryInstance(sm.type)));
        moduleInst->memAddrs.emplace_back(runtime.store().mems.size() - 1);
    }

    // global
    for (auto &sg : module.globals) {
        Value val = evaluateInitValue(sg.init);
        runtime.store().globals.push_back(Ptr<GlobalInstance>(new GlobalInstance(sg.type, val)));
        moduleInst->globalAddrs.emplace_back(runtime.store().globals.size() - 1);
    }

    // export
    for (auto &se : module.exports) {
        if (se.name.length() <= 0) {
            std::cout << "invalid exports name\n";
            return false;
        }

        ExternalValAddr ev = se.desc;
        switch(se.desc.type) {
            case ExtTypeFunc:   ev.addr = moduleInst->funcAddrs[ev.addr];   break;
            case ExtTypeTable:  ev.addr = moduleInst->tableAddrs[ev.addr];  break;
            case ExtTypeMem:    ev.addr = moduleInst->memAddrs[ev.addr];    break;
            case ExtTypeGlobal: ev.addr = moduleInst->globalAddrs[ev.addr]; break;
        }

        moduleInst->exports[se.name] = ExportInstance(se.name, ev);
    }

    // init table with element segment
    for (auto &se : module.elems) {
        Ptr<TableInstance> &tableInst = runtime.store().tables[moduleInst->tableAddrs[se.tableIdx]];
        Value offset = evaluateInitValue(se.offset);
        if (offset.i32 + se.init.size() > tableInst->elem.size()) {
            std::cout << "element segment offset exceed\n";
            return false;
        }

        for (U32 idx = 0; idx < se.init.size(); idx++) {
            tableInst->elem[idx + offset.i32] = moduleInst->funcAddrs[se.init[idx]];
        }
    }

    // init memory with data segment
    for (auto &sd : module.datas) {
        Ptr<MemoryInstance> &memInst = runtime.store().mems[moduleInst->memAddrs[sd.memIdx]];
        Value offset = evaluateInitValue(sd.offset);
        if ((U64)offset.i32 + sd.init.size() > memInst->dataSize) {
            std::cout << "data segment offset exceed\n";
            return false;
        }

        for (U32 idx = 0; idx < sd.init.size(); idx++) {
            memInst->data[idx + offset.i32] = sd.init[idx];
        }
    }

    // start func
    if (module.start.valid) {
        Addr funcAddr = moduleInst->funcAddrs[module.start.funcIdx];
        bool invokeOk = runtime.invokeStartFunc(funcAddr);
        if (!invokeOk) {
            std::cout << "invoke start function failed\n";
            return false;
        }
    }

    return true;
}

Value Instantiation::evaluateInitValue(ExprCodes &expr) {
    if (expr.size != 2 || expr.codes[1]->opCode != Op_end) {
        std::cout << "only support one instruction: " << expr.size << "\n";
        return Value();
    }

    BaseInstr *ins = expr.codes[0];
    switch (ins->opCode) {
        case Op_i32_const:
        case Op_i64_const:
        case Op_f32_const:
        case Op_f64_const:
            return ((NumericConstInstr *) ins)->val;
        default:
            std::cout << "init instruction not support: " << ins->opCode << "\n";
            break;
    }

    return Value();
}

}