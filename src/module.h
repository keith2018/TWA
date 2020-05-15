//
// Created by Keith on 2019/12/19.
//

#pragma once

#include <vector>
#include <string>
#include "twa.h"
#include "instruction.h"


namespace TWA {

struct SegFunc {
    U32 typeIdx;
    U32 localCnt;   // not include params cnt
    ExprCodes codes;
};

struct SegTable {
    TableType type;
};

struct SegMemory {
    MemType type;
};

struct SegGlobal {
    GlobalType type;
    ExprCodes init;
};

struct SegStartFunc {
    bool valid;
    U32 funcIdx;
};

struct SegElem {
    U32 tableIdx;
    ExprCodes offset;
    std::vector<U32> init;
};

struct SegData {
    U32 memIdx;
    ExprCodes offset;
    std::vector<U8> init;
};

struct SegImport {
    std::string module;
    std::string name;
    ExternalVal desc;
};

struct SegExport {
    std::string name;
    ExternalValAddr desc;
};

class Module {
public:
    Module() {
        this->start.valid = false;
    }

    ~Module() {
        for (SegGlobal &sg : globals) {
            releaseExprCodes(sg.init);
        }
        for (SegElem &se : elems) {
            releaseExprCodes(se.offset);
        }
        for (SegData &sd : datas) {
            releaseExprCodes(sd.offset);
        }
    }

public:
    std::string             name;
    U32                     version;

    std::vector<FuncType>   types;

    std::vector<SegFunc>    funcs;
    std::vector<SegTable>   tables;
    std::vector<SegMemory>  mems;
    std::vector<SegGlobal>  globals;

    std::vector<SegElem>    elems;
    std::vector<SegData>    datas;
    SegStartFunc            start;

    std::vector<SegImport>  imports;
    std::vector<SegExport>  exports;
};


}
