#ifndef __CURST_NUMERICS_H__
#define __CURST_NUMERICS_H__


#include <stddef.h>
#include <inttypes.h>
#include "curst.h"

#define unit _T()
typedef _t(unit) _unit;
const _unit UNIT = {};

typedef int8_t    _i8;
typedef int16_t   _i16;
typedef int32_t   _i32;
typedef int64_t   _i64;
typedef ptrdiff_t _isize;
typedef uint8_t   _u8;
typedef uint16_t  _u16;
typedef uint32_t  _u32;
typedef uint64_t  _u64;
typedef size_t    _usize;
typedef char      _bool;
typedef char      _char;
typedef float     _f32;
typedef double    _f64;

#define _for_all_numerics(M)\
    M(i8) M(i16) M(i32) M(i64) M(isize)\
    M(u8) M(u16) M(u32) M(u64) M(usize)\
    M(bool) M(char) M(f32) M(f64)

#define i8    (i8,  _i8,    _visit_mono(i08))
#define i16   (i16, _i16,   _visit_mono(i16))
#define i32   (i32, _i32,   _visit_mono(i32))
#define i64   (i64, _i64,   _visit_mono(i64))
#define isize (isz, _isize, _visit_mono(isz))
#define u8    (u8,  _u8,    _visit_mono(u08))
#define u16   (u16, _u16,   _visit_mono(u16))
#define u32   (u32, _u32,   _visit_mono(u32))
#define u64   (u64, _u64,   _visit_mono(u64))
#define usize (usz, _usize, _visit_mono(usz))
#define bool  (b,   _bool,  _visit_mono(bol))
#define char  (c,   _char,  _visit_mono(chr))
#define f32   (f32, _f32,   _visit_mono(f32))
#define f64   (f64, _f64,   _visit_mono(f64))

#define _visit_i08_VISITOR(...)
#define _visit_i16_VISITOR(...)
#define _visit_i32_VISITOR(...)
#define _visit_i64_VISITOR(...)
#define _visit_isz_VISITOR(...)
#define _visit_u08_VISITOR(...)
#define _visit_u16_VISITOR(...)
#define _visit_u32_VISITOR(...)
#define _visit_u64_VISITOR(...)
#define _visit_usz_VISITOR(...)
#define _visit_bol_VISITOR(...)
#define _visit_chr_VISITOR(...)
#define _visit_f32_VISITOR(...)
#define _visit_f64_VISITOR(...)

#define _visit_i08_debug(this) printf("%d",      *this)
#define _visit_i16_debug(this) printf("%d",      *this)
#define _visit_i32_debug(this) printf("%"PRId32, *this)
#define _visit_i64_debug(this) printf("%"PRId64, *this)
#define _visit_isz_debug(this) printf("%zd",     *this)
#define _visit_u08_debug(this) printf("%u",      *this)
#define _visit_u16_debug(this) printf("%u",      *this)
#define _visit_u32_debug(this) printf("%"PRIu32, *this)
#define _visit_u64_debug(this) printf("%"PRIu64, *this)
#define _visit_usz_debug(this) printf("%zu",     *this)
#define _visit_bol_debug(this) printf(*this ? "true" : "false")
#define _visit_chr_debug(this) printf("'%c'",    *this)
#define _visit_f32_debug(this) printf("%f",      *this)
#define _visit_f64_debug(this) printf("%f",      *this)
#define VISITOR debug
_for_all_numerics(_mono)
#undef VISITOR

#define _visit_i08_eq(this, other) *this == *other
#define _visit_i16_eq(this, other) *this == *other
#define _visit_i32_eq(this, other) *this == *other
#define _visit_i64_eq(this, other) *this == *other
#define _visit_isz_eq(this, other) *this == *other
#define _visit_u08_eq(this, other) *this == *other
#define _visit_u16_eq(this, other) *this == *other
#define _visit_u32_eq(this, other) *this == *other
#define _visit_u64_eq(this, other) *this == *other
#define _visit_usz_eq(this, other) *this == *other
#define _visit_bol_eq(this, other) *this == *other
#define _visit_chr_eq(this, other) *this == *other
#define _visit_f32_eq(this, other) *this == *other
#define _visit_f64_eq(this, other) *this == *other
#define VISITOR eq
_for_all_numerics(_mono)
#undef VISITOR

#define _visit_i08_clone(this) *this
#define _visit_i16_clone(this) *this
#define _visit_i32_clone(this) *this
#define _visit_i64_clone(this) *this
#define _visit_isz_clone(this) *this
#define _visit_u08_clone(this) *this
#define _visit_u16_clone(this) *this
#define _visit_u32_clone(this) *this
#define _visit_u64_clone(this) *this
#define _visit_usz_clone(this) *this
#define _visit_bol_clone(this) *this
#define _visit_chr_clone(this) *this
#define _visit_f32_clone(this) *this
#define _visit_f64_clone(this) *this
#define VISITOR clone
_for_all_numerics(_mono)
#undef VISITOR

#define _visit_i08_drop(this)
#define _visit_i16_drop(this)
#define _visit_i32_drop(this)
#define _visit_i64_drop(this)
#define _visit_isz_drop(this)
#define _visit_u08_drop(this)
#define _visit_u16_drop(this)
#define _visit_u32_drop(this)
#define _visit_u64_drop(this)
#define _visit_usz_drop(this)
#define _visit_bol_drop(this)
#define _visit_chr_drop(this)
#define _visit_f32_drop(this)
#define _visit_f64_drop(this)
#define VISITOR drop
_for_all_numerics(_mono)
#undef VISITOR


#endif // __CURST_NUMERICS_H__
