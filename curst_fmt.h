#ifndef __CURST_FMT_H__
#define __CURST_FMT_H__


#include <stdarg.h>
#include "curst_std.h"

// write: write a 0 terminated string
#define _visitor_write(...) __VA_ARGS__
#define _visitor_write_args t, fmt
#define _visitor_write_ret(Tt) _i32
#define _visitor_write_sig(Tt) (Tt *t, const _char* fmt)
#define _forward_write(V, _t, _fmt) ({_shadow(t, _t); _shadow(fmt, _fmt); V;})

// writef: interpolate all valid printf formatters
#define _visitor_writef(...) __VA_ARGS__
#define _visitor_writef_args t, fmt, args
#define _visitor_writef_ret(Tt) _i32
#define _visitor_writef_sig(Tt) (Tt *t, const _char *fmt, va_list args)
#define _forward_writef(V, _t, _fmt, _args) ({_shadow(t, _t); _shadow(fmt, _fmt); _shadow(args, _args); V;})

// fmt: write the representation of an instance into the given Formatter
#define _visitor_fmt(...) __VA_ARGS__
#define _visitor_fmt_args t, f
#define _visitor_fmt_ret(Tt) _i32
#define _visitor_fmt_sig(Tt) (Tt *t, Formatter *f)
#define _forward_fmt(V, _t, _f) ({_shadow(t, _t); _shadow(f, _f); V;})

//================================================
// Formatter definition and implementations
//================================================

typedef struct Formatter {
    void *data;
    _i32 (*writef)(void*, const _char*, va_list);
    _i32 (*write)(void*, const _char*);
    _i32 flags;
    _i32 depth;
} Formatter;

// Create Formatter from anything that implements write and writef
#define Formatter_new(T, x) _w(_Formatter_new, _split T, x)
// TODO: typechecking
#define _Formatter_new(Tn, Tt, Tv, x) (Formatter){\
    .data = (x),\
    .flags = 0, .depth = 0,\
    .write = (void*)Tn##_write,\
    .writef = (void*)Tn##_writef }

_i32 _write(Formatter *f, const _char *fmt) {
    return Dyn_call(write, f, fmt);
}
_i32 _writef(Formatter *f, const _char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _i32 n = Dyn_call(writef, f, fmt, args);
    va_end(args);
    return n;
}

_i32 _write_vfmt(Formatter *f, const _char *fmt, va_list args) {
    _i32 len = strlen(fmt);
    _char *p = malloc(len);
    memcpy(p, fmt, len);
    fmt = p;
    _i32 n = 0;
    while (*p) {
        if (p[0] == '%' && p[1] == '%') {
            p += 2;
            continue;
        }
        if (p[0] == '%' && p[1] == '?') {
            *p = 0;
            n += Dyn_call(writef, f, fmt, args);
            fmt = ++p + 1;
            let(l, va_arg(args, _fat_ptr));
            n += Fat_call(fmt, l, f);
        }
        p++;
    }
    n += Dyn_call(writef, f, fmt, args);
    return n;
}
// like _writef but interpolates %? with the given fmt implementation
_i32 _write_fmt(Formatter *f, const _char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _i32 n = _write_vfmt(f, fmt, args);
    va_end(args);
    return n;
}

//================================================
// fmt implementations
//================================================
#define _visit_i08_fmt(this, f) _writef(f, "%d",      *this)
#define _visit_i16_fmt(this, f) _writef(f, "%d",      *this)
#define _visit_i32_fmt(this, f) _writef(f, "%"PRId32, *this)
#define _visit_i64_fmt(this, f) _writef(f, "%"PRId64, *this)
#define _visit_isz_fmt(this, f) _writef(f, "%zd",     *this)
#define _visit_u08_fmt(this, f) _writef(f, "%u",      *this)
#define _visit_u16_fmt(this, f) _writef(f, "%u",      *this)
#define _visit_u32_fmt(this, f) _writef(f, "%"PRIu32, *this)
#define _visit_u64_fmt(this, f) _writef(f, "%"PRIu64, *this)
#define _visit_usz_fmt(this, f) _writef(f, "%zu",     *this)
#define _visit_bol_fmt(this, f) _writef(f, *this ? "true" : "false")
#define _visit_chr_fmt(this, f) _writef(f, "'%c'",    *this)
#define _visit_f32_fmt(this, f) _writef(f, "%f",      *this)
#define _visit_f64_fmt(this, f) _writef(f, "%f",      *this)
#define VISITOR fmt
_for_all_numerics(_mono)
#undef VISITOR

#define fn(T, ...) ({T __res; void *__ret_label=&&__ret_label; __VA_ARGS__; __ret_label: __res;})
#define _return(X) do { __res = X; goto *__ret_label; } while(0)
//#define _q(...) ({ __res=__VA_ARGS__; if(res.__tag != 0) goto *__ret_label; __res.Ok; })
#define _q(...) __VA_ARGS__

#define _Iter_fmt(Tv, len, this, f) fn(_i32,\
	_i32 res = _q(_write(f, "[\n"));\
	f->depth++;\
	for (_i32 i=0; i<len; i++) {\
		res += _q(_writef(f, "%*.s", f->depth*4, ""));\
		res += _q(_forward(Tv, this->as_ptr+i, f));\
		res += _q(_write(f, ",\n"));\
	}\
	f->depth--;\
	res += _q(_writef(f, "%*.s]", f->depth*4, ""));\
	_return(res);\
)
#define _visit__struct_fmt(N, Fs) fn(_i32,\
	_i32 res = _q(_write(f, #N " {\n"));\
	f->depth++;\
	DO(_visit_struct_fmt_each, Fs);\
	f->depth--;\
	res += _q(_writef(f, "%*.s}", f->depth*4, ""));\
	_return(res);\
)
#define _visit_struct_fmt
#define _visit_struct_fmt_each(i, F) _visit_struct_fmt##F
#define _visit_struct_fmt_field(N, T) res += _q(_writef(f, "%*.s" #N ": ", f->depth*4, "")); res += _q(_forward(__v T, &t->N, f)); res += _q(_write(f, ",\n"));

#define _visit__enum_fmt(N, Fs) fn(_i32,\
	_i32 res = _q(_write(f, #N "::"));\
	if (0) {}\
	DO(_visit_enum_fmt_each, Fs);\
	else _panic("unexpected tag in enum");\
	res += _q(_write(f, ")"));\
	_return(res);\
)
#define _visit_enum_fmt _panic("unexpected tag in enum")
#define _visit_enum_fmt_each(i, F) else if (t->__tag == i) { _visit_enum_fmt##F; }
#define _visit_enum_fmt_variant(N, T) res += _q(write(#N "(")); res += _q(_forward(__v T, &t->N, f))

#define _visit__tuple_fmt(Fs) fn(_i32,\
	_i32 res = _q(_write(f, "(\n"));\
	f->depth++;\
	DO(_visit_tuple_fmt_field, Fs);\
	f->depth--;\
	res += _q(_writef(f, "%*.s)" f->depth*4, ""));\
	_return(res);\
)
#define _visit_tuple_fmt_field(i, T) res += _q(_writef(f, "%*.s", f->depth*4, "")); res += _q(_forward(__v T, &t->_##i)); res += _q(_write(f, ",\n"));

#define _visit_Slc_fmt(Tv, this, f) _Iter_fmt(Tv, this->len, this, f)
#define _visit_Vec_fmt(Tv, this, f) _Iter_fmt(Tv, this->len, this, f)
#define _visit_Arr_fmt(N, Tv, this, f) _Iter_fmt(Tv, N, this, f)

//================================================
// write/writef implementations
//================================================

// String
#define VISITOR write
_impl(String, _This *this, const _char* strz) {
	let(str, Str_newz(strz));
	Vec_extend(this, str);
	return str.len;
}
#undef VISITOR

#define VISITOR writef
_impl(String, _This *this, const _char *fmt, va_list args) {
    va_list o_args;
    va_copy(o_args, args);
    _i32 n = vsnprintf(NULL, 0, fmt, args);
    Vec_reserve(this, n);
    n = vsnprintf(this->as_ptr + this->len, this->cap - this->len, fmt, o_args);
    this->len += n;
    va_end(o_args);
    return n;
}
#undef VISITOR

// File
#define VISITOR write
_impl(File, _This *this, const _char *strz) {
    return fprintf(this, strz);
}
#undef VISITOR

#define VISITOR writef
_impl(File, _This *this, const _char *fmt, va_list args) {
    return vfprintf(this, fmt, args);
}
#undef VISITOR

//================================================
// User API
//================================================

// String
_i32 String_write_vfmt(_t(String) *this, _bool nl, const _char *fmt, va_list args) {
    Formatter f = Formatter_new(String, this);
    _i32 n = _write_vfmt(&f, fmt, args);
    if (nl) {
        n += Dyn_call(write, &f, "\n");
    }
    return n;
}
_i32 String_writeln_fmt(_t(String) *this, const _char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _i32 n = String_write_vfmt(this, true, fmt, args);
    va_end(args);
    return n;
}
_i32 String_write_fmt(_t(String) *this, const _char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _i32 n = String_write_vfmt(this, false, fmt, args);
    va_end(args);
    return n;
}

// File
_i32 vfprint(FILE *this, _bool nl, const _char *fmt, va_list args) {
    Formatter f = Formatter_new(File, this);
    _i32 n = _write_vfmt(&f, fmt, args);
    if (nl) {
        n += fprintf(this, "\n");
    }
    return n;
}

_i32 println(const _char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _i32 n = vfprint(stdout, true, fmt, args);
    va_end(args);
    return n;
}
_i32 print(const _char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _i32 n = vfprint(stdout, false, fmt, args);
    va_end(args);
    return n;
}
_i32 eprintln(const _char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _i32 n = vfprint(stderr, true, fmt, args);
    va_end(args);
    return n;
}
_i32 eprint(const _char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _i32 n = vfprint(stderr, false, fmt, args);
    va_end(args);
    return n;
}
_i32 fprintln(FILE *this, const _char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _i32 n = vfprint(this, true, fmt, args);
    va_end(args);
    return n;
}
_i32 fprint(FILE *this, const _char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _i32 n = vfprint(this, false, fmt, args);
    va_end(args);
    return n;
}


#endif // __CURST_FMT_H__
