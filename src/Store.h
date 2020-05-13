//
// Created by Keith on 2019/12/27.
//

#ifndef TWA_STORE_H
#define TWA_STORE_H

#include <map>
#include <iostream>
#include "TWA.h"
#include "Instruction.h"
#include "Module.h"

namespace TWA {

struct ModuleInstance;
struct Store;
class Runtime;

struct FuncInstance {
    FuncInstance(ModuleInstance *m, bool isNative) {
        module = m;
        native = isNative;
    }

    ModuleInstance *module = nullptr;
    FuncType type;

    // wasm
    U32 localCnt = 0;
    ExprCodes codes;

    // native
    bool native = false;
    virtual bool nativeCall(Runtime &rt) = 0;
};

struct WasmFuncInstance : public FuncInstance {
    WasmFuncInstance(std::vector<FuncType> &t, SegFunc &sf, ModuleInstance *m)
            : FuncInstance(m, false) {
        type = t[sf.typeIdx];
        localCnt = sf.localCnt;
        codes = sf.codes;
    }

    bool nativeCall(Runtime &rt) {
        std::cout << "error call wasm function\n";
        return false;
    }
};

struct TableInstance {
    TableInstance(TableType &t) {
        type = t;
        elem = std::vector<Addr>(type.limit.min, '\0');
    }

    bool grow(U32 n) {
        U32 len = n + elem.size();
        if (len > TWA_MAX_TABLE_LEN) {
            std::cout << "len > TWA_MAX_TABLE_LEN\n";
            return false;
        }

        if (type.limit.max > 0 && len > type.limit.max) {
            std::cout << "len > max size\n";
            return false;
        }

        std::vector<Addr> appendVec(n, '\0');
        elem.insert(elem.end(), appendVec.begin(), appendVec.end());
        return true;
    }

    TableType type;
    std::vector<Addr> elem;
};

struct MemoryInstance {
    MemoryInstance(MemType &t) {
        type = t;
        pageCnt = t.min;
        dataSize = pageCnt * WasmPageSize;
        data = (U8 *) calloc(dataSize, sizeof(U8));
    }

    ~MemoryInstance() {
        free(data);
        data = nullptr;
    }

    bool grow(U32 n) {
        U32 newPageCnt = n + pageCnt;
        if (newPageCnt > TWA_MAX_MEMORY_LEN) {
            std::cout << "len > TWA_MAX_MEMORY_LEN\n";
            return false;
        }

        if (type.max > 0 && newPageCnt > type.max) {
            std::cout << "len > max size\n";
            return false;
        }

        U8 *p = (U8 *) realloc(data, newPageCnt * WasmPageSize);
        if (p == nullptr) {
            std::cout << "memory grow failed\n";
            return false;
        }
        data = p;
        memset(data + (pageCnt * WasmPageSize), 0, n * WasmPageSize);
        pageCnt = newPageCnt;
        dataSize = pageCnt * WasmPageSize;

        return true;
    }

    MemType type;
    U8 *data = nullptr;
    U32 pageCnt;
    U32 dataSize;
};

struct GlobalInstance {
    GlobalInstance(GlobalType &t, Value &v) {
        type = t;
        value = v;
    }

    GlobalType type;
    Value value;
};

struct ExportInstance {
    explicit ExportInstance() {}

    ExportInstance(std::string &n, ExternalValAddr &ev) {
        name = n;
        extVal = ev;
    }

    ExportInstance(const char *str, ExtType type, Addr addr) {
        name = str;
        extVal.type = type;
        extVal.addr = addr;
    }

    std::string name;
    ExternalValAddr extVal;
};

struct ModuleInstance {
    std::vector<FuncType> types;
    std::vector<Addr> funcAddrs;
    std::vector<Addr> tableAddrs;
    std::vector<Addr> memAddrs;
    std::vector<Addr> globalAddrs;
    std::map<std::string, ExportInstance> exports;
};

struct Store {
    std::vector<Ptr<FuncInstance>>      funcs;
    std::vector<Ptr<TableInstance>>     tables;
    std::vector<Ptr<MemoryInstance>>    mems;
    std::vector<Ptr<GlobalInstance>>    globals;

    ~Store() {
        for (Ptr<FuncInstance> &fi : funcs) {
            releaseExprCodes(fi->codes);
        }
    }
};

}
#endif //TWA_STORE_H
