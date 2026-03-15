#ifndef __CURST_STD_H__
#define __CURST_STD_H__


#include <stdlib.h>
#include <string.h>
#include "curst_numerics.h"

#ifdef _panic
#undef _panic
#endif
#define _panic(M) do {\
    fflush(stdout);\
    fprintf(stderr, "\npanicked at %s:%d, " M "\n", __FILE__, __LINE__);\
    exit(-1);\
} while(0)

// ===== Box =====
#define Box(T) _w(_Box, _split T)
#define _Box(Tn, Tt, Tv) (_b##Tn, struct _b##Tn { const Tt *as_ptr; }, _visit_poly(Box, Tv))
#define _visit_Box_VISITOR(...)
// ----- debug -----
#define _visit_Box_debug(Tv, this) _forward(Tv, this->as_ptr)
// ----- eq -----
#define _visit_Box_eq(Tv, this, other) _forward(Tv, this->as_ptr, other->as_ptr)
// ----- drop -----
#define _visit_Box_drop(Tv, this) _forward(Tv, this->as_ptr)
// ----- clone -----
#define _visit_Box_clone(Tv, this) ({\
    typeof(this) res;\
    *res->as_ptr = _forward(Tv, this->as_ptr);\
    res;\
})

// ===== Reference =====
#define Ref(T) _w(_Ref, _split T)
#define _Ref(Tn, Tt, Tv) (_r##Tn, struct _r##Tn { const Tt *as_ptr }, _visit_poly(Ref, Tv))
// ----- debug -----
#define _visit_Ref_debug(Tv, this) _forward(Tv, this->as_ptr)
// ----- eq -----
#define _visit_Ref_eq(Tv, this, other) _forward(Tv, this->as_ptr, other->as_ptr)
// ----- drop -----
#define _visit_Ref_drop(Tv, this)
// ----- clone -----
#define _visit_Ref_clone(Tv, this) *this

// ===== Mutable Reference =====
#define RefMut(T) _w(_RefMut, _split T)
#define _RefMut(Tn, Tt, Tv) (_rm##Tn, struct _r##Tn { Tt *as_ptr }, _visit_poly(Ref, Tv)) // use Ref visitors

// ===== Counted Reference =====
#define Rc(T) _w(_Rc, _split T)
#define _Rc(Tn, Tt, Tv) (_rc##Tn, struct _rc##Tn { _u32 *strong; const Tt *as_ptr; }, _visit_poly(Rc, Tv))
// ----- debug -----
#define _visit_Rc_debug(Tv, this) _forward(Tv, this->as_ptr)
// ----- eq -----
#define _visit_Rc_eq(Tv, this, other) _forward(Tv, this->as_ptr, other->as_ptr)
// ----- drop -----
#define _visit_Rc_drop(Tv, this) ({ if (--this->strong == 0) _forward(Tv, this->as_ptr); })
// ----- clone -----
#define _visit_Rc_clone(Tv, this) ({ this->strong++; *this; })



// ===== Common Iterator implementations ====
// ----- debug -----
#define _Iter_debug(Tv, len, this) ({\
    printf("[ ");\
    for (int i = 0; i < len; i++) {\
        _forward(Tv, this->as_ptr+i);\
        printf(", ");\
    }\
    printf("]");\
})
// ----- eq -----
#define _Iter_eq(Tv, this_len, other_len, this, other) ({\
    if (this_len == other_len) {\
        _bool res = true;\
        for (int i = 0; i < this_len; i++) {\
        _bool item_res = _forward(Tv, this->as_ptr+i, other->as_ptr+i);\
        if (!item_res) { res = false; break; }\
        }\
    } else { res = false; }\
    res;\
})

struct Range {
    _isize start;
    _usize len;
};

#define _in ,
#define _range(start, end) (struct Range){ .start = start, .len = (start)-(len) }
#define _for(i, iter) for(_usize __o=1;__o;) for(typeof(iter)__iter=iter;__o;__o=0) for(_usize __i=0,__iter_len=_len(__iter);__i<__iter_len;__i++) for(_item(iter);__o;)
#define _enumerate(i) for(_usize i=__i;__o;__o=0)
#define _filter(...) if(!(__VA_ARGS__))else


// ===== Slice ====
#define Slc(T) _w(_Slc, _split T)
#define _Slc(Tn, Tt, Tv) (_s##Tn, struct _s##Tn { _u32 len; Tt *as_ptr; }, _visit_poly(Slc, Tv))
// ----- debug -----
#define _visit_Slc_debug(Tv, this) _Iter_debug(Tv, this->len, this)
// ----- eq -----
#define _visit_Slc_eq(Tv, this, other) _Iter_eq(Tv, this->len, other->len, this, other)
// ----- clone -----
#define _visit_Slc_clone(Tv, this) *this
// ----- drop -----
#define _visit_Slc_drop(Tv, this)

#define _slice_type(Tn, Tt) __attribute__((packed)) struct _s##Tn { _u32 len; Tt *as_ptr; } __slice_type[0]
#define _as_slice_type(x) typeof(*(x)->__slice_type)
#define _as_slice(x) (_as_slice_type(x)){ .as_ptr=(x)->as_ptr, .len=_len(x) }
#define _len(x) _Generic((x)->len,\
    _u32: (x)->len,\
    default: sizeof((x)->as_ptr)/sizeof(*(x)->as_ptr))



// ===== String Slice =====
#define Str Slc(char)
#define Str_new(x) (_t(Str)){ .len = sizeof(x)-1, .as_ptr = (x) }
#define Str_newz(x) (_t(Str)){ .len = strlen(x), .as_ptr = (_char*)(x) }

// ===== Vector =====
#define Vec(T) _w(_Vec, _split T)
#define _Vec(Tn, Tt, Tv) (_v##Tn, struct _v##Tn {\
        _slice_type(Tn, Tt);\
        _u32 cap; _u32 len; Tt *as_ptr;\
    }, _visit_poly(Vec, Tv))
#define _visit_Vec_VISITOR(...)
// ----- debug -----
#define _visit_Vec_debug(Tv, this) _Iter_debug(Tv, this->len, this)
// ----- eq -----
#define _visit_Vec_eq(Tv, this, other) _Iter_eq(Tv, this->len, other->len, this, other)
// ----- clone -----
#define _visit_Vec_clone(Tv, this) ({\
    typeof(*this) res = {};\
    res.len = this->len;\
    res.cap = res.len;\
    res.as_ptr = malloc(res.len * sizeof(*this->as_ptr));\
    if (!res.as_ptr) _panic("Vec_clone: malloc failed");\
    for (int i = 0; i < res.len; i++) {\
        res.as_ptr[i] = _forward(Tv, this->as_ptr+i);\
    }\
    res;\
})
// ----- drop -----
#define _visit_Vec_drop(Tv, this) ({\
    for (int i = 0; i < this->len; i++) {\
        _forward(Tv, this->as_ptr+i);\
    }\
    free(this->as_ptr);\
    *this = (typeof(*this)){};\
})

// ----- Implementations -----
#define Vec_new(T) (_t(Vec(T))){}
#define Vec_with_capacity(T, n) ({\
    _t(Vec(T)) res = { .len = 0, .cap = n };\
    res.as_ptr = malloc(n * sizeof(*res.as_ptr));\
    if (!res.as_ptr) _panic("Vec_new: malloc failed");\
    res;\
})
#define Vec_reserve(_this, _n) do {\
    _shadow(this, _this);\
    _shadow(n, _n);\
    if (this->len + n >= this->cap) {\
    this->cap = this->cap ? 2*this->cap : 4;\
    while (this->len + n >= this->cap) this->cap *= 2;\
        this->as_ptr = realloc(this->as_ptr, this->cap * sizeof(*this->as_ptr));\
        if (!this->as_ptr) _panic("Vec_push: realloc failed");\
    }\
} while(0)
#define Vec_push(_this, val) do {\
    _shadow(this, _this);\
    Vec_reserve(this, 1);\
    this->as_ptr[this->len++] = val;\
} while(0)
#define Vec_extend(_this, _slc) do {\
	_shadow(this, _this);\
	_shadow(slc, _slc);\
	Vec_reserve(this, slc.len);\
	memcpy(this->as_ptr + this->len, slc.as_ptr, slc.len * sizeof(*slc.as_ptr));\
	this->len += slc.len;\
} while(0)

// ===== String =====
// #define String (_vc, struct _vc {\
//         _slice_type(c, _char);\
//         _u32 cap; _u32 len; _char *as_ptr;\
//     }, _visit_mono(Sring))
// #define _visit_String_VISITOR(...)
// // ----- debug -----
// #define _visit_String_debug(Tv, this) printf("%.*s", this->len, this->as_ptr)
// // ----- eq -----
// #define _visit_String_eq(Tv, this, other) _Iter_eq(Tv, this->len, other->len, this, other)
// // ----- clone -----
// #define _visit_Vec_clone(Tv, this) ({\
// })
// // ----- drop -----
// #define _visit_Vec_drop(Tv, this) ({\
//     free(this->as_ptr);\
//     *this = (typeof(*this)){};\
// })
#define String Vec(char)
#define String_new() Vec_new(char)
#define String_with_capacity(n) Vec_with_capacity(char, n)

// ===== Array =====
#define Arr(N, T) _w(_Arr, N, _split T)
#define _Arr(N, Tn, Tt, Tv) (_a##N##Tn, struct _a##N##Tn {\
        Tt as_ptr[N];\
        _u8 len[0];\
        _slice_type(Tn, Tt);\
    }, _visit_poly(Arr, N, Tv))
#define _visit_Arr_VISITOR(...)
// ----- debug -----
#define _visit_Arr_debug(N, Tv, this) _Iter_debug(Tv, N, this)
// ----- eq -----
#define _visit_Arr_eq(N, Tv, this, other) _Iter_eq(Tv, N, N, this, other)
// ----- clone -----
#define _visit_Arr_clone(N, Tv, this) ({\
    typeof(*this) res = {};\
    for (int i = 0; i < N; i++) {\
        res.as_ptr[i] = _forward(Tv, this->as_ptr+i);\
    }\
    res;\
})
// ----- drop -----
#define _visit_Arr_drop(N, Tv, this) ({\
    for (int i = 0; i < N; i++) {\
        _forward(Tv, this->as_ptr+i);\
    }\
})

// ===== HashMap =====
#define HashMap(K, V) _w(_HashMap, _split K, _split V)
#define _HashMap(Kn, Kt, Kv, Vn, Vt, Vv) (_hm##Kn##Vn, struct _hm##Kn##Vn {\
    _u32 n_buckets; _u32 len;\
    struct _hb##Kn##Vn { _u32 len; struct _hn##Kn##Vn { struct _hn##Kn##Vn *next; Kt key; Vt val; } *head; } *buckets;\
}, _visit_poly(HashMap, Kv, Vv))
#define _visit_HashMap_VISITOR(...)
// ----- debug -----
#define _visit_HashMap_debug(Kv, Vv, this) ({\
    printf("{ ");\
    for (int i = 0; i < this->n_buckets; i++) {\
        let(node, this->buckets[i].head);\
        while (node) {\
            _forward(Kv, &node->key);\
            printf(": ");\
            _forward(Vv, &node->val);\
            node = node->next;\
            printf(", ");\
        }\
    }\
    printf("}");\
})
// ----- clone -----
#define _visit_HashMap_clone(Kv, Vv, this) ({\
    typeof(*this) res = { .n_buckets = this->n_buckets, .len = this->len };\
    res.buckets = calloc(res.n_buckets, sizeof(*res.buckets));\
    if (!res.buckets) _panic("HashMap_clone: malloc failed");\
    for (int i = 0; i < res.n_buckets; i++) {\
        res.buckets[i].len = this->buckets[i].len;\
        let(node, this->buckets[i].head);\
        let(prev, &res.buckets[i].head);\
        while (node) {\
            typeof(node) new = malloc(sizeof(*node));\
            new->key = _forward(Kv, &node->key);\
            new->val = _forward(Vv, &node->val);\
            new->next = NULL;\
            *prev = new;\
            prev = &new->next;\
            node = node->next;\
        }\
    }\
    res;\
})
// ----- drop -----
#define _visit_HashMap_drop(Kv, Vv, this) ({\
    for (int i = 0; i < this->n_buckets; i++) {\
        let(node, this->buckets[i].head);\
        while (node) {\
            _forward(Kv, &node->key);\
            _forward(Vv, &node->val);\
            node = node->next;\
        }\
    }\
    free(this->buckets);\
    *this = (typeof(*this)){};\
})

// ----- Implementations -----
// TODO: use hash and eq traits
#define HashMap_new(K, V) HashMap_with_buckets(K, V, 10)
#define HashMap_with_buckets(K, V, n) ({\
    _t(HashMap(K, V)) res = { .n_buckets = n, .len = 0 };\
    res.buckets = calloc(n, sizeof(*res.buckets));\
    if (!res.buckets) _panic("HashMap_with_buckets: malloc failed");\
    res;\
})
#define HashMap_insert(_this, _k, _v) ({\
    _shadow(this, _this); _shadow(k, _k); _shadow(v, _v);\
    int idx = k % this->n_buckets;\
    let(node, this->buckets[idx].head);\
    _bool found = false;\
    while (node) {\
        if (node->key == k) {\
            node->val = v;\
            found = true;\
            break;\
        }\
        node = node->next;\
    }\
    if (!found) {\
        node = malloc(sizeof(*node));\
        if (!node) _panic("HashMap_insert: malloc failed");\
        node->key = k;\
        node->val = v;\
        node->next = this->buckets[idx].head;\
        this->buckets[idx].head = node;\
        this->buckets[idx].len++;\
        this->len++;\
    }\
    &node->val;\
})
#define HashMap_get(_this, _k) ({\
    _shadow(this, _this); _shadow(k, _k);\
    int idx = k % this->n_buckets;\
    let(node, this->buckets[idx].head);\
    typeof(&node->val) res = NULL;\
    while (node) {\
        if (node->key == k) {\
            res = &node->val;\
            break;\
        }\
        node = node->next;\
    }\
    res;\
})

// ===== Result =====
enum Res_tags { Res_Ok=0, Res_Err=1 };
#define Res(T, E) _w(VISITOR, T, E)
#define _Res(V, Tn, Tt, Tv, En, Et, Ev) (_r##Tn##En, struct _r##Tn##En { enum Res_tags __tags; union { Tt Ok; Et Err; }; }, _visitor_Res_##V(Tv, Ev, _visit_##V##_args))
#define _visit_Res_VISITOR(...)
// ----- debug -----
#define _visit_Res_debug(Tv, Ev, this) ({\
    if (this->__tag == 0) {\
        printf("Res::Ok(");\
        _forward(Tv, &this->Ok);\
    } else {\
        printf("Res::Err(");\
        _forward(Ev, &this->Err);\
    }\
    printf(")");\
})
// ----- eq -----
#define _visit_Res_eq(Tv, Ev, this, other) ({\
    if (this->__tag == 0 && other.__tag == 0)\
        _forward(Tv, &this->Ok, &other->Ok);\
    else if (this->__tag == other->__tag)\
        _forward(Ev, &this->Err, &other->Err);\
    else false;\
})
// ----- clone -----
#define _visit_Res_clone(Tv, Ev, this) ({\
    typeof(*this) res = { .__tag=this->__tag };\
    if (res.__tag == 0)\
        res.Ok = _forward(Tv, &this->Ok);\
    else\
        res.Err = _forward(Ev, &this->Err);\
    res;\
})
// ----- drop -----
#define _visit_Res_drop(Tv, Ev, this) ({\
    if (this->__tag == 0)\
        _forward(Tv, &this->Ok);\
    else\
        _forward(Ev, &this->Err);\
})

// ===== Option =====
enum Opt_tags { Opt_Some=0, Opt_None=1 };
#define Opt(T) _w(VISITOR, T)
#define _Opt(V, Tn, Tt, Tv) (_o##Tn, struct _o##Tn { enum Opt_tags __tags; union { Tt Some; _unit None; }; }, _visitor_Opt_##V(Tv, _visit_##V##_args))
#define _visit_Opt_VISITOR(...)
// ----- debug -----
#define _visit_Opt_debug(Tv, this) ({\
    if (this->__tag == 0) {\
        printf("Opt::Some(");\
        _forward(Tv, &this->Some);\
        printf(")");\
    else\
        printf("Opt::None");\
})
// ----- eq -----
#define _visit_Opt_eq(Tv, this, other) ({\
    if (this->__tag == 0 && other.__tag == 0)\
        _forward(Tv, &this->Some, &other->Some);\
    else if (this->__tag == other->__tag) true;\
    else false;\
})
// ----- clone -----
#define _visit_Opt_clone(Tv, this) ({\
    typeof(*this) res = { .__tag=this->__tag };\
    if (res.__tag == 0)\
        res.Some = _forward(Tv, &this->Some);\
    res;\
})
// ----- drop -----
#define _visit_Opt_drop(Tv, this) ({\
    if (this->__tag == 0)\
        _forward(Tv, &this->Some);\
})

#define File (_f, FILE, _visit_mono(File))
#define _visit_File_VISITOR(...)
// ----- debug -----
#define _visit_File_debug()


// ===== Dynamic =====
// TODO: field decl
#define Dyn(...) _Dyn1(_cat2(_d, CAT(DO(_mangle_C, (__VA_ARGS__)))), VISITOR, (__VA_ARGS__))
#define _Dyn1(...) _Dyn2(__VA_ARGS__)
#define _Dyn2(M, V, Fs) (M, struct M { DO(_decl_dyn_method, Fs) }, _visit_##V(_visit_Dyn(V, _visitor_##V##_args)))
// ----- Dyn derives all its visitors -----
#define _visitor_Dyn(V, this, ...) Dyn_call(V, *this, this, __VA_ARGS__)

// ----- Implementations -----
#define Dyn_new(T, Fs, x) _t(Dyn Fs){  }
#define Dyn_call(V, _x, ...) ({\
	_shadow(__x, _x);\
	_visitor_##V##_ret(__NOT_DYN_COMPATIBLE__) res = (_visitor_typeof(V, void)__x->V)(__x->data, __VA_ARGS__);\
	res;\
})
#define Dyn_into(V, _x) ({\
	_shadow(__x, _x);\
	_fat_ptr res = { .data = __x.data, .func = __x.V };\
	res;\
})

// ----- Fat Pointer -----
typedef struct Fat {
	void *data;
	void *func;
} _fat_ptr;
#define Fat_new(T, V, x) (_fat_ptr){ .data = (x), .func = _m(T, V) }
#define Fat_call(V, _x, ...) ({\
	_shadow(__x, _x);\
	_visitor_##V##_ret(__NOT_DYN_COMPATIBLE__) res = (_visitor_typeof(V, void)__x.func)(__x.data, __VA_ARGS__);\
	res;\
})


#endif // __CURST_STD_H__
