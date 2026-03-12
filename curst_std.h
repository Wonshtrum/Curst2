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

// ===== Vec =====
#define Vec(T) _w(_Vec, _split T)
#define _Vec(Tn, Tt, Tv) (_v##Tn, struct _v##Tn { _u32 cap; _u32 len; Tt *ptr; }, _visit_poly(Vec, Tv))
#define _visit_Vec_VISITOR(...)
// ----- debug -----
#define _visit_Vec_debug(Tv, this) ({\
    printf("[ ");\
    for (int i = 0; i < this->len; i++) {\
        _forward(Tv, this->ptr+i);\
        printf(", ");\
    }\
    printf("]"); _void;\
})
// ----- eq -----
#define _visit_Vec_eq(Tv, this, other) ({\
    _bool res = true;\
    if (this->len == other->len) {\
        for (int i = 0; i < this->len; i++) {\
            _bool item_res = _forward(Tv, this->ptr+i, other->ptr+i);\
            if (!item_res) { res = false; break; }\
        }\
    } else { res = false; }\
    res;\
})
// ----- clone -----
#define _visit_Vec_clone(Tv, this) ({\
    typeof(*this) res = { 0 };\
    res.len = this->len;\
    res.cap = res.len;\
    res.ptr = malloc(res.len * sizeof(*this->ptr));\
    if (!res.ptr) _panic("Vec_clone: malloc failed");\
    for (int i = 0; i < res.len; i++) {\
        res.ptr[i] = _forward(Tv, this->ptr+i);\
    }\
    res;\
})
// ----- drop -----
#define _visit_Vec_drop(Tv, this) ({\
    for (int i = 0; i < this->len; i++) {\
        _forward(Tv, this->ptr+i);\
    }\
    free(this->ptr);\
    *this = (typeof(*this)){ 0 };\
    _void;\
})

// ----- Implementations -----
#define Vec_new(T) (_t(Vec(T))){ 0 }
#define Vec_with_capacity(T, n) ({\
    _t(Vec(T)) res = { .len = 0, .cap = n };\
    res.ptr = malloc(n * sizeof(*res.ptr));\
    if (!res.ptr) _panic("Vec_new: malloc failed");\
    res;\
})
#define Vec_push(_this, val) do {\
    _shadow(this, _this);\
    /* printf("Vec_push: len=%d, cap=%d\n", this->len, this->cap);*/\
    if (this->len >= this->cap) {\
        this->cap = this->cap ? 2*this->cap : 4;\
        this->ptr = realloc(this->ptr, this->cap * sizeof(*this->ptr));\
        if (!this->ptr) _panic("Vec_push: realloc failed");\
    }\
    this->ptr[this->len++] = val;\
} while(0)

// ===== Arr =====
#define Arr(N, T) _w(_Arr, N, _split T)
#define _Arr(N, Tn, Tt, Tv) (_a##N##Tn, typeof(Tt[N]), _visit_poly(Arr, N, Tv))
#define _visit_Arr_VISITOR(...)
// ----- debug -----
#define _visit_Arr_debug(N, Tv, this) ({\
    printf("[ ");\
    for (int i = 0; i < N; i++) {\
        _forward(Tv, *this+i);\
        printf(", ");\
    }\
    printf("]"); _void;\
})
// ----- eq -----
#define _visit_Arr_eq(N, Tv, this, other) ({\
    _bool res = true;\
    for (int i = 0; i < N; i++) {\
        _bool item_res = _forward(Tv, *this+i, *other+i);\
        if (!item_res) { res = false; break; }\
    }\
    res;\
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
    printf("}"); _void;\
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
    *this = (typeof(*this)){ 0 };\
    _void;\
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


#endif // __CURST_STD_H__
