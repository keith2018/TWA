//
// Created by Keith on 2019/12/31.
//

#pragma once

#include <cmath>
#include "twa.h"


namespace TWA {
namespace Utils {

inline I32 signExt_8_32(I8 val) {
    if (val & 0x80) {
        return (I32) val | (I32) 0xffffff00;
    }
    return val;
}

inline I32 signExt_16_32(I16 val) {
    if (val & 0x8000) {
        return (I32) val | (I32) 0xffff0000;
    }
    return val;
}

inline I64 signExt_8_64(I8 val) {
    if (val & 0x80) {
        return (I64) val | (I64) 0xffffffffffffff00;
    }
    return val;
}

inline I64 signExt_16_64(I16 val) {
    if (val & 0x8000) {
        return (I64) val | (I64) 0xffffffffffff0000;
    }
    return val;
}

inline I64 signExt_32_64(I32 val) {
    if (val & (I32) 0x80000000) {
        return (I64) val | (I64) 0xffffffff00000000;
    }
    return val;
}

inline U32 CLZ(U32 value) {
#if defined(__GNUC__)
    return value == 0 ? 32 : __builtin_clz(value);
#else
    U32 n = 0;
    if (type == 0)
        return 32;
    while (!(type & 0x80000000)) {
        n++;
        type <<= 1;
    }
    return n;
#endif
}

inline U64 CLZ(U64 value) {
#if defined(__GNUC__)
    return value == 0 ? 64 : __builtin_clzll(value);
#else
    U32 n = 0;
    if (type == 0)
        return 64;
    while (!(type & 0x8000000000000000LL)) {
        n++;
        type <<= 1;
    }
    return n;
#endif
}

inline U32 CTZ(U32 value) {
#if defined(__GNUC__)
    return value == 0 ? 32 : __builtin_ctz(value);
#else
    U32 n = 0;
    if (type == 0)
        return 32;
    while (!(type & 1)) {
        n++;
        type >>= 1;
    }
    return n;
#endif
}

inline U64 CTZ(U64 value) {
#if defined(__GNUC__)
    return value == 0 ? 64 : __builtin_ctzll(value);
#else
    U32 n = 0;
    if (type == 0)
        return 64;
    while (!(type & 1)) {
        n++;
        type >>= 1;
    }
    return n;
#endif
}

inline U32 POPCNT(U32 value) {
#if defined(__GNUC__)
    return value == 0 ? 0 : __builtin_popcount(value);
#else
    U32 ret = 0;
    while (u) {
        u = (u & (u - 1));
        ret++;
    }
    return ret;
#endif
}

inline U64 POPCNT(U64 value) {
#if defined(__GNUC__)
    return value == 0 ? 0 : __builtin_popcountll(value);
#else
    U32 ret = 0;
    while (u) {
        u = (u & (u - 1));
        ret++;
    }
    return ret;
#endif
}

inline U32 ROTL_32(U32 n, U32 c) {
    const U32 mask = (31);
    c = c % 32;
    c &= mask;
    return (n << c) | (n >> ((-c) & mask));
}

inline U32 ROTR_32(U32 n, U32 c) {
    const U32 mask = (31);
    c = c % 32;
    c &= mask;
    return (n >> c) | (n << ((-c) & mask));
}

inline U64 ROTL_64(U64 n, U64 c) {
    const U64 mask = (63);
    c = c % 64;
    c &= mask;
    return (n << c) | (n >> ((-c) & mask));
}

inline U64 ROTR_64(U64 n, U64 c) {
    const U64 mask = (63);
    c = c % 64;
    c &= mask;
    return (n >> c) | (n << ((-c) & mask));
}

inline F32 MIN_F32(F32 a, F32 b) {
    if (unlikely(std::isnan(a)) || unlikely(std::isnan(b))) return NAN;
    if (a == 0 and a == b) return std::signbit(a) ? a : b;
    return a > b ? b : a;
}

inline F32 MAX_F32(F32 a, F32 b) {
    if (unlikely(std::isnan(a)) || unlikely(std::isnan(b))) return NAN;
    if (a == 0 and a == b) return std::signbit(a) ? b : a;
    return a > b ? a : b;
}

inline F64 MIN_F64(F64 a, F64 b) {
    if (unlikely(std::isnan(a)) || unlikely(std::isnan(b))) return NAN;
    if (a == 0 and a == b) return std::signbit(a) ? a : b;
    return a > b ? b : a;
}

inline F64 MAX_F64(F64 a, F64 b) {
    if (unlikely(std::isnan(a)) || unlikely(std::isnan(b))) return NAN;
    if (a == 0 and a == b) return std::signbit(a) ? b : a;
    return a > b ? a : b;
}

}
}
