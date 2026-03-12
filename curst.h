#ifndef __CURST_H__
#define __CURST_H__


#include "do.h"

// no std panic
#define _panic(M) do { extern void exit(int); exit(-1); } while(0)

// ===== Utils =====
#define let(v, x) typeof(x) v = x

#define _name(T) _n(T)
#define _type(T) _t(T)
#define _mangle(T, V) _m(T, V)
#define _n(T) _with(__n, _split T)
#define _t(T) _with(__t, _split T)
#define _m(T, V) _cat2(__n T, _##V)
#define __n(Tn, Tt, Tv) Tn
#define __t(Tn, Tt, Tv) Tt
#define __v(Tn, Tt, Tv) Tv

#define _cat2(A, B) __cat2(A, B)
#define __cat2(A, B) A##B
#define _cat3(A, B, C) __cat3(A, B, C)
#define __cat3(A, B, C) A##B##C

#define _with(M, ...) M(__VA_ARGS__)
#define _w(M, ...) M(__VA_ARGS__)
#define _split(...) __VA_ARGS__

// ===== Visitors =====
#define _visitor(I) _cat3(_visitor_, VISITOR, _##I)
#define _visit_mono(T) _with(_cat2(_visit_##T##_, VISITOR), _visitor(args))
#define _visit_poly(T, ...) _with(_cat2(_visit_##T##_, VISITOR), __VA_ARGS__, _visitor(args))
#define _visit(T, ...) _forward(__v T, __VA_ARGS__)

#define _void (void)0
#define _forward(V, ...) _cat2(_forward_, VISITOR)(V, __VA_ARGS__)
#define _shadow(v, x) typeof(x) _##v = x; typeof(_##v) v = _##v
#define _derive(Tn, Tv, ...) Tv
#define _dispatch(Tn, Tv, ...) _with(__dispatch, Tn, VISITOR, _visitor(args))
#define __dispatch(Tn, V, ...) Tn##_##V(__VA_ARGS__)
#define _call(V, T, ...) _cat2(_n(T), _##V)(__VA_ARGS__)

#define _mono(T) _with(__mono, VISITOR, _split T)
#define __mono(V, Tn, Tt, Tv) _visitor(ret)(Tt) Tn##_##V _visitor(sig)(Tt) { return Tv; }
#define _mono_with_alias(T, A) _with(__mono_with_alias, A, VISITOR, _split T)
#define __mono_with_alias(A, V, Tn, Tt, Tv)\
    _visitor(ret)(Tt) Tn##_##V _visitor(sig)(Tt) { return Tv; }\
    _visitor(ret)(Tt) A _visitor(sig)(Tt) { return Tn##_##V(_visitor(args)); }
#define _This typeof(*this)
#define _impl(T, ...) _with(__impl, VISITOR, _split T, __VA_ARGS__)
#define __impl(V, Tn, Tt, Tv, ...)\
    _visitor(ret)(Tt) Tn##_##V _visitor(sig)(Tt);\
    _visitor(ret)(Tt) Tn##_##V (Tt *this; __VA_ARGS__)
#define _impl_with_alias(T, A, ...) _with(__impl_with_alias, A, VISITOR, _split T, __VA_ARGS__)
#define __impl_with_alias(A, V, Tn, Tt, Tv, ...)\
    _visitor(ret)(Tt) A _visitor(sig)(Tt);\
    _visitor(ret)(Tt) Tn##_##V _visitor(sig)(Tt) { return A(_visitor(args)); }\
    _visitor(ret)(Tt) A (Tt *this; __VA_ARGS__)

#include "curst_visit.h"

// ===== Structures =====
#define _mangle_NCT(N, Cs, Ts) CAT(N DO(_mangle_C, Cs) DO(_mangle_T, Ts))
#define _mangle_T(i, T) ,__n T
#define _mangle_C(i, C) ,_##C
#define _struct(N, Cs, Ts, ...) _struct1(_mangle_NCT(N, Cs, Ts), VISITOR, N, Cs, Ts, (__VA_ARGS__))
#define _struct1(...) _struct2(__VA_ARGS__)
#define _struct2(M, V, N, Cs, Ts, Fs) (M, struct M { DO(_decl_struct_each, Fs) }, _visitor_##V(_visit_##N##_##V(M, _visit__struct_##V(N, Fs), _split Cs, _split Ts, _visitor_##V##_args)))
#define _decl_struct
#define _decl_struct_each(i, F) _decl_struct##F
#define _decl_struct_field(N, T) __t T N;
// ----- debug -----
#define _visit__struct_debug(N, Fs) ({\
    printf(#N " { ");\
    DO(_visit_struct_debug_each, Fs);\
    printf("}"); _void;\
})
#define _visit_struct_debug
#define _visit_struct_debug_each(i, F) _visit_struct_debug##F
#define _visit_struct_debug_field(N, T) printf(#N ": "); _forward(__v T, &t->N); printf(", ");
// ----- eq -----

// ===== Enumerations =====
#define _enum(N, Cs, Ts, ...) _enum1(_mangle_NCT(N, Cs, Ts), VISITOR, N, Cs, Ts, (__VA_ARGS__))
#define _enum1(...) _enum2(__VA_ARGS__)
#define _enum2(M, V, N, Cs, Ts, Fs) (M, typeof(struct M { enum N##_tags { DO_WITH(_decl_tags_each, N, Fs) } __tag; union { DO(_decl_enum_each, Fs) }; }), _visitor_##V(_visit_##N##_##V(M, _visit__enum_##V(N, Fs), _split Cs, _split Ts, _visitor_##V##_args)))
#define _decl_enum
#define _decl_enum_each(i, F) _decl_enum##F
#define _decl_enum_variant(N, T) __t T N;
#define _decl_tags _IGNORE,
#define _decl_tags_each(i, N, F) _decl_tags_variant1(i, N, _decl_tags##F)
#define _decl_tags_variant(...) __VA_ARGS__
#define _decl_tags_variant1(...) _decl_tags_variant2(__VA_ARGS__)
#define _decl_tags_variant2(i, N, F, T) N##_##F = i,
// ----- debug -----
#define _visit__enum_debug(N, Fs) ({\
    printf(#N "::");\
    if (false) {}\
    DO(_visit_enum_debug_each, Fs)\
    else {};\
    printf(""); _void;\
})
#define _visit_enum_debug _panic("unexepected tag in enum")
#define _visit_enum_debug_each(i, F) else if (t->__tag == i) { _visit_enum_debug##F; }
#define _visit_enum_debug_variant(N, T) printf(#N "("); _forward(__v T, &t->N); printf(")")
// ----- eq -----

// ===== Tuples =====
#define _T(...) _tuple(__VA_ARGS__)
#define _tuple(...) _tuple1(CAT(_q DO(_mangle_T, (__VA_ARGS__)), p), VISITOR, (__VA_ARGS__))
#define _tuple1(...) _tuple2(__VA_ARGS__)
#define _tuple2(M, V, Fs) (M, struct M { DO(_decl_tuple_field, Fs) }, _visit__tuple_##V(Fs))
#define _decl_tuple_field(i, T) __t T _##i;
#define _visit__tuple_VISITOR(...)
#define _visit__tuple_debug(Fs) ({\
    printf("( ");\
    DO(_visit_tuple_debug_field, Fs);\
    printf(")"); _void;\
})
#define _visit_tuple_debug_field(i, T) _forward(__v T, &t->_##i); printf(", ");


#endif // __CURST_H__
