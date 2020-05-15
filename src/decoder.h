//
// Created by Keith on 2019/12/18.
//

#pragma once

#include <iostream>
#include "twa.h"
#include "module.h"

namespace TWA {

enum SectionId : U8 {
    SectionCustom = 0,
    SectionType = 1,
    SectionImport = 2,
    SectionFunction = 3,
    SectionTable = 4,
    SectionMemory = 5,
    SectionGlobal = 6,
    SectionExport = 7,
    SectionStart = 8,
    SectionElement = 9,
    SectionCode = 10,
    SectionData = 11,
    SectionLength = 12
};

enum SectionState {
    SectionStateFail,
    SectionStateSuccess,
    SectionStateEOF
};

#define CUSTOM_SECTION_NAME "name"

enum NameSectionId : U8 {
    NameSectionModule = 0,
    NameSectionFunction = 1,
    NameSectionLocal = 2,
};

class Decoder {
public:
    explicit Decoder(Module &module);

    bool decodeStream(std::istream &stream);
    bool decodeFile(std::string &filePath);
    bool decodeBytes(const U8 *bytes);

private:
    bool readMagic();
    bool readVersion();
    SectionState readSection();

    // section decode
    bool readSection_custom(U32 len);
    bool readSection_type(U32 len);
    bool readSection_import(U32 len);
    bool readSection_function(U32 len);
    bool readSection_table(U32 len);
    bool readSection_memory(U32 len);
    bool readSection_global(U32 len);
    bool readSection_export(U32 len);
    bool readSection_start(U32 len);
    bool readSection_element(U32 len);
    bool readSection_code(U32 len);
    bool readSection_data(U32 len);

private:
    template<typename T>
    T readElement();
    void readFuncSignature(FuncSignature &sig);
    std::string readName();
    bool readBoolValue();

    // read types
    FuncType    readFuncType();
    Limit       readLimit();
    MemType     readMemType();
    TableType   readTableType();
    GlobalType  readGlobalType();

    // read instructions
    void readExpr(ExprCodes &expr, U32 size);
    void readControlInstr(U8 opCode, BaseInstr **ins);
    void readNumericConstInstr(U8 opCode, NumericConstInstr *ins);

    // leb128 integer
    U32 readLebU32();
    I32 readLebI32();
    U64 readLebU64();
    I64 readLebI64();

private:
    bool (Decoder::*secFunctions[SectionLength])(U32);
    std::istream *_file;
    Module &_module;
};

}
