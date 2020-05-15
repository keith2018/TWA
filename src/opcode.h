//
// Created by Keith on 2019/12/20.
//

#pragma once

#include "twa.h"

namespace TWA {

// control
static const U8 Op_unreachable          = 0x00;
static const U8 Op_nop                  = 0x01;
static const U8 Op_block                = 0x02;
static const U8 Op_loop                 = 0x03;
static const U8 Op_if                   = 0x04;
static const U8 Op_else                 = 0x05;

static const U8 Op_end                  = 0x0B;
static const U8 Op_br                   = 0x0C;
static const U8 Op_br_if                = 0x0D;
static const U8 Op_br_table             = 0x0E;
static const U8 Op_return               = 0x0F;
static const U8 Op_call                 = 0x10;
static const U8 Op_call_indirect        = 0x11;

// parametric
static const U8 Op_drop                 = 0x1A;
static const U8 Op_select               = 0x1B;

// variable
static const U8 Op_get_local            = 0x20;
static const U8 Op_set_local            = 0x21;
static const U8 Op_tee_local            = 0x22;
static const U8 Op_get_global           = 0x23;
static const U8 Op_set_global           = 0x24;

// memory
static const U8 Op_i32_load		        = 0x28;
static const U8 Op_i64_load		        = 0x29;
static const U8 Op_f32_load		        = 0x2A;
static const U8 Op_f64_load		        = 0x2B;
static const U8 Op_i32_load8_s		    = 0x2C;
static const U8 Op_i32_load8_u		    = 0x2D;
static const U8 Op_i32_load16_s	        = 0x2E;
static const U8 Op_i32_load16_u	        = 0x2F;
static const U8 Op_i64_load8_s		    = 0x30;
static const U8 Op_i64_load8_u		    = 0x31;
static const U8 Op_i64_load16_s	        = 0x32;
static const U8 Op_i64_load16_u	        = 0x33;
static const U8 Op_i64_load32_s	        = 0x34;
static const U8 Op_i64_load32_u	        = 0x35;
static const U8 Op_i32_store		    = 0x36;
static const U8 Op_i64_store		    = 0x37;
static const U8 Op_f32_store		    = 0x38;
static const U8 Op_f64_store		    = 0x39;
static const U8 Op_i32_store8		    = 0x3A;
static const U8 Op_i32_store16		    = 0x3B;
static const U8 Op_i64_store8		    = 0x3C;
static const U8 Op_i64_store16		    = 0x3D;
static const U8 Op_i64_store32		    = 0x3E;
static const U8 Op_memory_size		    = 0x3F;
static const U8 Op_memory_grow		    = 0x40;

// numeric
static const U8 Op_i32_const		    = 0x41;
static const U8 Op_i64_const		    = 0x42;
static const U8 Op_f32_const		    = 0x43;
static const U8 Op_f64_const		    = 0x44;

static const U8 Op_i32_eqz		        = 0x45;
static const U8 Op_i32_eq		        = 0x46;
static const U8 Op_i32_ne		        = 0x47;
static const U8 Op_i32_lt_s		        = 0x48;
static const U8 Op_i32_lt_u		        = 0x49;
static const U8 Op_i32_gt_s		        = 0x4A;
static const U8 Op_i32_gt_u		        = 0x4B;
static const U8 Op_i32_le_s		        = 0x4C;
static const U8 Op_i32_le_u		        = 0x4D;
static const U8 Op_i32_ge_s		        = 0x4E;
static const U8 Op_i32_ge_u		        = 0x4F;
static const U8 Op_i64_eqz		        = 0x50;
static const U8 Op_i64_eq		        = 0x51;
static const U8 Op_i64_ne		        = 0x52;
static const U8 Op_i64_lt_s		        = 0x53;
static const U8 Op_i64_lt_u		        = 0x54;
static const U8 Op_i64_gt_s		        = 0x55;
static const U8 Op_i64_gt_u		        = 0x56;
static const U8 Op_i64_le_s		        = 0x57;
static const U8 Op_i64_le_u		        = 0x58;
static const U8 Op_i64_ge_s		        = 0x59;
static const U8 Op_i64_ge_u		        = 0x5A;
static const U8 Op_f32_eq		        = 0x5B;
static const U8 Op_f32_ne		        = 0x5C;
static const U8 Op_f32_lt		        = 0x5D;
static const U8 Op_f32_gt		        = 0x5E;
static const U8 Op_f32_le		        = 0x5F;
static const U8 Op_f32_ge		        = 0x60;
static const U8 Op_f64_eq		        = 0x61;
static const U8 Op_f64_ne		        = 0x62;
static const U8 Op_f64_lt		        = 0x63;
static const U8 Op_f64_gt		        = 0x64;
static const U8 Op_f64_le		        = 0x65;
static const U8 Op_f64_ge               = 0x66;
static const U8 Op_i32_clz		        = 0x67;
static const U8 Op_i32_ctz		        = 0x68;
static const U8 Op_i32_popcnt		    = 0x69;
static const U8 Op_i32_add		        = 0x6A;
static const U8 Op_i32_sub		        = 0x6B;
static const U8 Op_i32_mul		        = 0x6C;
static const U8 Op_i32_div_s		    = 0x6D;
static const U8 Op_i32_div_u		    = 0x6E;
static const U8 Op_i32_rem_s		    = 0x6F;
static const U8 Op_i32_rem_u		    = 0x70;
static const U8 Op_i32_and		        = 0x71;
static const U8 Op_i32_or		        = 0x72;
static const U8 Op_i32_xor		        = 0x73;
static const U8 Op_i32_shl		        = 0x74;
static const U8 Op_i32_shr_s		    = 0x75;
static const U8 Op_i32_shr_u		    = 0x76;
static const U8 Op_i32_rotl		        = 0x77;
static const U8 Op_i32_rotr		        = 0x78;
static const U8 Op_i64_clz		        = 0x79;
static const U8 Op_i64_ctz		        = 0x7A;
static const U8 Op_i64_popcnt		    = 0x7B;
static const U8 Op_i64_add		        = 0x7C;
static const U8 Op_i64_sub		        = 0x7D;
static const U8 Op_i64_mul		        = 0x7E;
static const U8 Op_i64_div_s		    = 0x7F;
static const U8 Op_i64_div_u		    = 0x80;
static const U8 Op_i64_rem_s		    = 0x81;
static const U8 Op_i64_rem_u		    = 0x82;
static const U8 Op_i64_and		        = 0x83;
static const U8 Op_i64_or		        = 0x84;
static const U8 Op_i64_xor		        = 0x85;
static const U8 Op_i64_shl		        = 0x86;
static const U8 Op_i64_shr_s		    = 0x87;
static const U8 Op_i64_shr_u		    = 0x88;
static const U8 Op_i64_rotl		        = 0x89;
static const U8 Op_i64_rotr		        = 0x8A;
static const U8 Op_f32_abs		        = 0x8B;
static const U8 Op_f32_neg		        = 0x8C;
static const U8 Op_f32_ceil		        = 0x8D;
static const U8 Op_f32_floor		    = 0x8E;
static const U8 Op_f32_trunc		    = 0x8F;
static const U8 Op_f32_nearest		    = 0x90;
static const U8 Op_f32_sqrt		        = 0x91;
static const U8 Op_f32_add		        = 0x92;
static const U8 Op_f32_sub		        = 0x93;
static const U8 Op_f32_mul		        = 0x94;
static const U8 Op_f32_div		        = 0x95;
static const U8 Op_f32_min		        = 0x96;
static const U8 Op_f32_max		        = 0x97;
static const U8 Op_f32_copysign	        = 0x98;
static const U8 Op_f64_abs		        = 0x99;
static const U8 Op_f64_neg		        = 0x9A;
static const U8 Op_f64_ceil		        = 0x9B;
static const U8 Op_f64_floor		    = 0x9C;
static const U8 Op_f64_trunc		    = 0x9D;
static const U8 Op_f64_nearest		    = 0x9E;
static const U8 Op_f64_sqrt		        = 0x9F;
static const U8 Op_f64_add		        = 0xA0;
static const U8 Op_f64_sub		        = 0xA1;
static const U8 Op_f64_mul		        = 0xA2;
static const U8 Op_f64_div		        = 0xA3;
static const U8 Op_f64_min		        = 0xA4;
static const U8 Op_f64_max		        = 0xA5;
static const U8 Op_f64_copysign	        = 0xA6;
static const U8 Op_i32_wrap_i64	        = 0xA7;
static const U8 Op_i32_trunc_s_f32	    = 0xA8;
static const U8 Op_i32_trunc_u_f32	    = 0xA9;
static const U8 Op_i32_trunc_s_f64	    = 0xAA;
static const U8 Op_i32_trunc_u_f64	    = 0xAB;
static const U8 Op_i64_extend_s_i32     = 0xAC;
static const U8 Op_i64_extend_u_i32     = 0xAD;
static const U8 Op_i64_trunc_s_f32		= 0xAE;
static const U8 Op_i64_trunc_u_f32		= 0xAF;
static const U8 Op_i64_trunc_s_f64		= 0xB0;
static const U8 Op_i64_trunc_u_f64		= 0xB1;
static const U8 Op_f32_convert_s_i32	= 0xB2;
static const U8 Op_f32_convert_u_i32	= 0xB3;
static const U8 Op_f32_convert_s_i64	= 0xB4;
static const U8 Op_f32_convert_u_i64	= 0xB5;
static const U8 Op_f32_demote_f64		= 0xB6;
static const U8 Op_f64_convert_s_i32	= 0xB7;
static const U8 Op_f64_convert_u_i32	= 0xB8;
static const U8 Op_f64_convert_s_i64	= 0xB9;
static const U8 Op_f64_convert_u_i64	= 0xBA;
static const U8 Op_f64_promote_f32		= 0xBB;
static const U8 Op_i32_reinterpret_f32	= 0xBC;
static const U8 Op_i64_reinterpret_f64	= 0xBD;
static const U8 Op_f32_reinterpret_i32	= 0xBE;
static const U8 Op_f64_reinterpret_i64	= 0xBF;

static const U8 OpMaxLength	            = 0xFF;

#define REG_ALL_OP_FUNC     \
    REG_OP_FUNC(Op_unreachable);\
    REG_OP_FUNC(Op_nop);\
    REG_OP_FUNC(Op_block);\
    REG_OP_FUNC(Op_loop);\
    REG_OP_FUNC(Op_if);\
    \
    REG_OP_FUNC(Op_end);\
    REG_OP_FUNC(Op_br);\
    REG_OP_FUNC(Op_br_if);\
    REG_OP_FUNC(Op_br_table);\
    REG_OP_FUNC(Op_return);\
    REG_OP_FUNC(Op_call);\
    REG_OP_FUNC(Op_call_indirect);\
    \
    REG_OP_FUNC(Op_drop);\
    REG_OP_FUNC(Op_select);\
    \
    REG_OP_FUNC(Op_get_local);\
    REG_OP_FUNC(Op_set_local);\
    REG_OP_FUNC(Op_tee_local);\
    REG_OP_FUNC(Op_get_global);\
    REG_OP_FUNC(Op_set_global);\
    \
    REG_OP_FUNC(Op_i32_load);\
    REG_OP_FUNC(Op_i64_load);\
    REG_OP_FUNC(Op_f32_load);\
    REG_OP_FUNC(Op_f64_load);\
    REG_OP_FUNC(Op_i32_load8_s);\
    REG_OP_FUNC(Op_i32_load8_u);\
    REG_OP_FUNC(Op_i32_load16_s);\
    REG_OP_FUNC(Op_i32_load16_u);\
    REG_OP_FUNC(Op_i64_load8_s);\
    REG_OP_FUNC(Op_i64_load8_u);\
    REG_OP_FUNC(Op_i64_load16_s);\
    REG_OP_FUNC(Op_i64_load16_u);\
    REG_OP_FUNC(Op_i64_load32_s);\
    REG_OP_FUNC(Op_i64_load32_u);\
    \
    REG_OP_FUNC(Op_i32_store);\
    REG_OP_FUNC(Op_i64_store);\
    REG_OP_FUNC(Op_f32_store);\
    REG_OP_FUNC(Op_f64_store);\
    REG_OP_FUNC(Op_i32_store8);\
    REG_OP_FUNC(Op_i32_store16);\
    REG_OP_FUNC(Op_i64_store8);\
    REG_OP_FUNC(Op_i64_store16);\
    REG_OP_FUNC(Op_i64_store32);\
    \
    REG_OP_FUNC(Op_memory_size);\
    REG_OP_FUNC(Op_memory_grow);\
    \
    REG_OP_FUNC(Op_i32_const);\
    REG_OP_FUNC(Op_i64_const);\
    REG_OP_FUNC(Op_f32_const);\
    REG_OP_FUNC(Op_f64_const);\
    \
    REG_OP_FUNC(Op_i32_eqz);\
    REG_OP_FUNC(Op_i32_eq);\
    REG_OP_FUNC(Op_i32_ne);\
    REG_OP_FUNC(Op_i32_lt_s);\
    REG_OP_FUNC(Op_i32_lt_u);\
    REG_OP_FUNC(Op_i32_gt_s);\
    REG_OP_FUNC(Op_i32_gt_u);\
    REG_OP_FUNC(Op_i32_le_s);\
    REG_OP_FUNC(Op_i32_le_u);\
    REG_OP_FUNC(Op_i32_ge_s);\
    REG_OP_FUNC(Op_i32_ge_u);\
    REG_OP_FUNC(Op_i64_eqz);\
    REG_OP_FUNC(Op_i64_eq);\
    REG_OP_FUNC(Op_i64_ne);\
    REG_OP_FUNC(Op_i64_lt_s);\
    REG_OP_FUNC(Op_i64_lt_u);\
    REG_OP_FUNC(Op_i64_gt_s);\
    REG_OP_FUNC(Op_i64_gt_u);\
    REG_OP_FUNC(Op_i64_le_s);\
    REG_OP_FUNC(Op_i64_le_u);\
    REG_OP_FUNC(Op_i64_ge_s);\
    REG_OP_FUNC(Op_i64_ge_u);\
    REG_OP_FUNC(Op_f32_eq);\
    REG_OP_FUNC(Op_f32_ne);\
    REG_OP_FUNC(Op_f32_lt);\
    REG_OP_FUNC(Op_f32_gt);\
    REG_OP_FUNC(Op_f32_le);\
    REG_OP_FUNC(Op_f32_ge);\
    REG_OP_FUNC(Op_f64_eq);\
    REG_OP_FUNC(Op_f64_ne);\
    REG_OP_FUNC(Op_f64_lt);\
    REG_OP_FUNC(Op_f64_gt);\
    REG_OP_FUNC(Op_f64_le);\
    REG_OP_FUNC(Op_f64_ge);\
    REG_OP_FUNC(Op_i32_clz);\
    REG_OP_FUNC(Op_i32_ctz);\
    REG_OP_FUNC(Op_i32_popcnt);\
    REG_OP_FUNC(Op_i32_add);\
    REG_OP_FUNC(Op_i32_sub);\
    REG_OP_FUNC(Op_i32_mul);\
    REG_OP_FUNC(Op_i32_div_s);\
    REG_OP_FUNC(Op_i32_div_u);\
    REG_OP_FUNC(Op_i32_rem_s);\
    REG_OP_FUNC(Op_i32_rem_u);\
    REG_OP_FUNC(Op_i32_and);\
    REG_OP_FUNC(Op_i32_or);\
    REG_OP_FUNC(Op_i32_xor);\
    REG_OP_FUNC(Op_i32_shl);\
    REG_OP_FUNC(Op_i32_shr_s);\
    REG_OP_FUNC(Op_i32_shr_u);\
    REG_OP_FUNC(Op_i32_rotl);\
    REG_OP_FUNC(Op_i32_rotr);\
    REG_OP_FUNC(Op_i64_clz);\
    REG_OP_FUNC(Op_i64_ctz);\
    REG_OP_FUNC(Op_i64_popcnt);\
    REG_OP_FUNC(Op_i64_add);\
    REG_OP_FUNC(Op_i64_sub);\
    REG_OP_FUNC(Op_i64_mul);\
    REG_OP_FUNC(Op_i64_div_s);\
    REG_OP_FUNC(Op_i64_div_u);\
    REG_OP_FUNC(Op_i64_rem_s);\
    REG_OP_FUNC(Op_i64_rem_u);\
    REG_OP_FUNC(Op_i64_and);\
    REG_OP_FUNC(Op_i64_or);\
    REG_OP_FUNC(Op_i64_xor);\
    REG_OP_FUNC(Op_i64_shl);\
    REG_OP_FUNC(Op_i64_shr_s);\
    REG_OP_FUNC(Op_i64_shr_u);\
    REG_OP_FUNC(Op_i64_rotl);\
    REG_OP_FUNC(Op_i64_rotr);\
    REG_OP_FUNC(Op_f32_abs);\
    REG_OP_FUNC(Op_f32_neg);\
    REG_OP_FUNC(Op_f32_ceil);\
    REG_OP_FUNC(Op_f32_floor);\
    REG_OP_FUNC(Op_f32_trunc);\
    REG_OP_FUNC(Op_f32_nearest);\
    REG_OP_FUNC(Op_f32_sqrt);\
    REG_OP_FUNC(Op_f32_add);\
    REG_OP_FUNC(Op_f32_sub);\
    REG_OP_FUNC(Op_f32_mul);\
    REG_OP_FUNC(Op_f32_div);\
    REG_OP_FUNC(Op_f32_min);\
    REG_OP_FUNC(Op_f32_max);\
    REG_OP_FUNC(Op_f32_copysign);\
    REG_OP_FUNC(Op_f64_abs);\
    REG_OP_FUNC(Op_f64_neg);\
    REG_OP_FUNC(Op_f64_ceil);\
    REG_OP_FUNC(Op_f64_floor);\
    REG_OP_FUNC(Op_f64_trunc);\
    REG_OP_FUNC(Op_f64_nearest);\
    REG_OP_FUNC(Op_f64_sqrt);\
    REG_OP_FUNC(Op_f64_add);\
    REG_OP_FUNC(Op_f64_sub);\
    REG_OP_FUNC(Op_f64_mul);\
    REG_OP_FUNC(Op_f64_div);\
    REG_OP_FUNC(Op_f64_min);\
    REG_OP_FUNC(Op_f64_max);\
    REG_OP_FUNC(Op_f64_copysign);\
    REG_OP_FUNC(Op_i32_wrap_i64);\
    REG_OP_FUNC(Op_i32_trunc_s_f32);\
    REG_OP_FUNC(Op_i32_trunc_u_f32);\
    REG_OP_FUNC(Op_i32_trunc_s_f64);\
    REG_OP_FUNC(Op_i32_trunc_u_f64);\
    REG_OP_FUNC(Op_i64_extend_s_i32);\
    REG_OP_FUNC(Op_i64_extend_u_i32);\
    REG_OP_FUNC(Op_i64_trunc_s_f32);\
    REG_OP_FUNC(Op_i64_trunc_u_f32);\
    REG_OP_FUNC(Op_i64_trunc_s_f64);\
    REG_OP_FUNC(Op_i64_trunc_u_f64);\
    REG_OP_FUNC(Op_f32_convert_s_i32);\
    REG_OP_FUNC(Op_f32_convert_u_i32);\
    REG_OP_FUNC(Op_f32_convert_s_i64);\
    REG_OP_FUNC(Op_f32_convert_u_i64);\
    REG_OP_FUNC(Op_f32_demote_f64);\
    REG_OP_FUNC(Op_f64_convert_s_i32);\
    REG_OP_FUNC(Op_f64_convert_u_i32);\
    REG_OP_FUNC(Op_f64_convert_s_i64);\
    REG_OP_FUNC(Op_f64_convert_u_i64);\
    REG_OP_FUNC(Op_f64_promote_f32);\
    REG_OP_FUNC(Op_i32_reinterpret_f32);\
    REG_OP_FUNC(Op_i64_reinterpret_f64);\
    REG_OP_FUNC(Op_f32_reinterpret_i32);\
    REG_OP_FUNC(Op_f64_reinterpret_i64);\

}
