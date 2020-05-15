//
// Created by Keith on 2019/12/19.
//

#pragma once

#include <cstdint>
#include <vector>
#include <cstring>
#include <memory>

namespace TWA {

#define TWA_VERSION "0.0.1"
#define Ptr std::shared_ptr
#define MAX_STACK_SIZE 512*1024
#define MAX_FUNC_LOCAL_SIZE 4*1024

#if defined __GNUC__
#   define likely(x)   __builtin_expect (!!(x), 1)
#   define unlikely(x) __builtin_expect (!!(x), 0)
#else
#   define likely(x) (x)
#   define unlikely(x) (x)
#endif

typedef uint8_t     U8;
typedef int8_t      I8;
typedef uint16_t    U16;
typedef int16_t     I16;
typedef uint32_t    U32;
typedef int32_t     I32;
typedef uint64_t    U64;
typedef int64_t     I64;
typedef float       F32;
typedef double      F64;

static const U32 WasmMagic      = 0x6d736100; // "\0asm"
static const U32 WasmVersion    = 0x00000001;
static const U32 WasmPageSize   = 65536;      // 64 KiB
static const U8  WasmZeroByte   = 0x00;
static const U8  WasmTrue       = 0x01;
static const U8  WasmFalse      = 0x00;

static const U8  WasmValueTypeVoid       = 0x40;
static const U8  WasmValueTypeI32        = 0x7F;
static const U8  WasmValueTypeI64        = 0x7E;
static const U8  WasmValueTypeF32        = 0x7D;
static const U8  WasmValueTypeF64        = 0x7C;
static const U8  WasmValueTypeFunc       = 0x60;
static const U8  WasmValueTypeFuncRef    = 0x70;

static const U32 TWA_MAX_TABLE_LEN  = (2U << 31);   // 2^32
static const U32 TWA_MAX_MEMORY_LEN = (2U << 15);   // 2^16
static const U32 TWA_MAX_PARAM_LEN  = 32;

// value type
enum ValueType : U8 {
    ValueTypeNone       = 0,
    ValueTypeI32        = WasmValueTypeI32,
    ValueTypeI64        = WasmValueTypeI64,
    ValueTypeF32        = WasmValueTypeF32,
    ValueTypeF64        = WasmValueTypeF64,
    ValueTypeVoid       = WasmValueTypeVoid,
    ValueTypeFuncRef    = WasmValueTypeFuncRef
};

union Value {
    I32 i32;
    I64 i64;
    F32 f32;
    F64 f64;

    Value() : i64(0) {}
    Value(I32 val) { i32 = val; }
    Value(I64 val) { i64 = val; }
    Value(F32 val) { f32 = val; }
    Value(F64 val) { f64 = val; }
};

struct TypedValue {
    TypedValue() {}
    TypedValue(ValueType &t, Value &v) {
        type = t;
        val = v;
    }

    ValueType type;
    Value val;
};

struct FuncSignature {
    FuncSignature() : size(0) {}

    U32 size;
    ValueType types[TWA_MAX_PARAM_LEN];

    inline bool equals(const FuncSignature &f) const {
        if (f.size != size) {
            return false;
        }

        return memcmp(f.types, types, size) == 0;
    }
};

// function type
struct FuncType {
    FuncSignature params;
    ValueType retType = ValueTypeVoid;

    inline bool equals(const FuncType &f) const {
        return f.params.equals(params) && f.retType == retType;
    }
};

// limits
struct Limit {
    U32 min;
    U32 max;
};

// memory type
typedef Limit MemType;

struct TableType {
    ValueType   elemType;
    Limit       limit;
};

// global type
struct GlobalType {
    ValueType   valueType;
    bool        mut;
};

// addr
typedef U32 Addr;

// external
enum ExtType : U8 {
    ExtTypeFunc     = 0x00,
    ExtTypeTable    = 0x01,
    ExtTypeMem      = 0x02,
    ExtTypeGlobal   = 0x03
};

struct ExternalVal {
    ExtType type;
    union {
        U32         typeIdx;
        TableType   tableType;
        MemType     memType;
        GlobalType  globalType;
    };
};

struct ExternalValAddr {
    ExternalValAddr() {}
    ExternalValAddr(ExtType &t, Addr &a) {
        type = t;
        addr = a;
    }

    ExtType type;
    Addr addr;
};

enum RuntimeTrap {
    TrapNone,
    TrapExit,
    TrapUnreachable,
    TrapStackOverflow,
    TrapIntegerOverflow,
    TrapInvalidConversionToInteger,
    TrapIntegerDivideByZero,
    TrapOutOfBoundsMemoryAccess,
    TrapUndefinedElement,
    TrapIndirectCallTypeMismatch
};

static const char* TrapDesc[] = {
        "",
        "proc_exit",
        "unreachable executed",
        "stack overflow",
        "integer overflow",
        "invalid conversion to integer",
        "integer divide by zero",
        "out of bounds memory access",
        "undefined element",
        "indirect call type mismatch"
};

}
