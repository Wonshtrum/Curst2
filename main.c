#include <stdio.h>
#include "curst_std.h"
#include "curst_match.h"

#define MyTuple(A, B) _struct(\
    MyTuple, (), (A, B),\
    _field(a, A),\
    _field(b, B))
#define _visit_MyTuple_debug(...) _derive(__VA_ARGS__)
#define _visit_MyTuple_eq(...)    _derive(__VA_ARGS__)

#define Tree(N, T) _struct(\
    Tree, (N), (T),\
    _field(val, T),\
    _field(children, Arr(N, Box(_rec(Tree, (N), (T))))))

#define User _struct(\
    User, (), (),\
    _field(name, u32),\
    _field(age, u8),\
    _field(tag, i32))
#define _visit_User_debug(...) _derive(__VA_ARGS__)
#define _visit_User_clone(...) _derive(__VA_ARGS__)
#define _visit_User_drop(...)  _derive(__VA_ARGS__)
#define _visit_User_eq(...)    _derive(__VA_ARGS__)

#define Res(T, E) _enum(\
    Res, (), (T, E),\
    _variant(Ok, T),\
    _variant(Err, E),)
#define _visit_Res_debug(...) _derive(__VA_ARGS__)

#define Opt(T) _enum(\
    Opt, (), (T),\
    _variant(Some, T),\
    _variant(None, unit))
#define _visit_Opt_debug(...) _derive(__VA_ARGS__)

#define VISITOR debug
_mono_with_alias(Arr(2, i32), my_alias)
_impl_with_alias(MyTuple(i32, i32), mt_debug, _This *this) {
    printf("MyTuple(%d, %d)", this->a, this->b);
}
_mono(User)
#undef VISITOR

int main() {
    {
        _t(Vec(Vec(Vec(i32)))) t = { 0 }, o = { 0 };
        Vec_push(&t, Vec_new(Vec(i32)));
        Vec_push(&t.ptr[0], Vec_with_capacity(i32, 5));
        for (int i = 0; i < 10; i++) {
            Vec_push(&t.ptr[0].ptr[0], i);
        }
        #define VISITOR debug
        _visit(Vec(Vec(Vec(i32))), &t);
        printf("\n");
        _visit(Vec(Vec(Vec(i32))), &o);
        #undef VISITOR
        printf("\n");
        #define VISITOR eq
        _bool res = _visit(Vec(Vec(Vec(i32))), &t, &o);
        #undef VISITOR
        printf("eq: %d\n",res);
    }
    {
        _t(Arr(2, Arr(3, i32))) t = { { 1, 2, 3 }, { 4, 5, 6 } };
        #define VISITOR debug
        _visit(Arr(2, Arr(3, i32)), &t);
        #undef VISITOR
        printf("\n");
    }
    {
        _t(Arr(3, i32)) t = { 1, 2, 3 };
        #define VISITOR debug
        _visit(Arr(3, i32), &t);
        #undef VISITOR
        printf("\n");
    }
    {
        _t(Arr(2, i32)) t;
        _call(debug, Arr(2, i32), &t);
        printf("\n");
        my_alias(&t);
        printf("\n");
    }
    {
        _t(MyTuple(i32, i32)) t = { .a = 1, .b = 2 };
        mt_debug(&t);
        printf("\n");
        _call(debug, MyTuple(i32, i32), &t);
        printf("\n");
        #define VISITOR debug
        _visit(MyTuple(i32, i32), &t);
        #undef VISITOR
        printf("\n");
    }
    {
        _t(MyTuple(i32, i64)) t = { 2, 3 };
        #define VISITOR debug
        _visit(MyTuple(i32, i64), &t);
        #undef VISITOR
        printf("\n");
    }
    {
        _t(_T(i32, _T(i32))) t = { ._0 = 42, ._1 = 1337 };
        #define VISITOR debug
        _visit(_T(i32, _T(i32)), &t);
        #undef VISITOR
        printf("\n");
    }
    {
        _t(User) u = { .age = 100 };
        #define VISITOR debug
        _visit(User, &u);
        #undef VISITOR
        printf("\n");
        _call(debug, User, &u);
        printf("\n");
        User_debug(&u);
        printf("\n");
    }
    {
        _t(usize) u = 123;
        #define VISITOR debug
        _visit(usize, &u);
        #undef VISITOR
        printf("\n");
        #define VISITOR eq
        _bool res = _visit(usize, &u, &u);
        #undef VISITOR
        #define VISITOR clone
        _t(usize) u2 = _visit(usize, &u);
        #undef VISITOR
        #define VISITOR drop
        _visit(usize, &u);
        #undef VISITOR
    }
    {
        let(h, HashMap_new(i32, chr));
        for (int i = 0; i < 10; i++) {
            HashMap_insert(&h, i, '0'+i);
            #define VISITOR debug
            _visit(HashMap(i32, chr), &h);
            #undef VISITOR
            printf("\n");
        }
        #define VISITOR clone
        let(h2, _visit(HashMap(i32, chr), &h));
        #undef VISITOR
        printf("h2: ");
        #define VISITOR debug
        _visit(HashMap(i32, chr), &h2);
        #undef VISITOR
        printf("\n");
        for (int i = 0; i < 12; i++) {
            char *v = HashMap_get(&h, i);
            char *v2 = HashMap_get(&h2, i);
            if (v) {
                printf("h[%d] = '%c'\n", i, *v);
            } else {
                printf("h[%d] = (nil)\n", i);
            }
            if (v2) {
                printf("h2[%d] = '%c'\n", i, *v);
            } else {
                printf("h2[%d] = (nil)\n", i);
            }
        }
    }
    {
        let(h, HashMap_new(i32, Vec(chr)));
        for (int i = 0; i < 10; i++) {
            let(v, Vec_with_capacity(chr, i));
            for (int j = 0; j < i; j++) {
                Vec_push(&v, '0'+j);
            }
            HashMap_insert(&h, i, v);
        }
        #define VISITOR debug
        _visit(HashMap(i32, Vec(chr)), &h);
        #undef VISITOR
        printf("\n");
        HashMap_get(&h, 5)->ptr[3] = '@';
        for (int i = 0; i < 10; i++) {
            let(v, HashMap_get(&h, i));
            printf("h[%d]: ", i);
            #define VISITOR debug
            _visit(Vec(chr), v);
            #undef VISITOR
            printf("\n");
        }
        #define VISITOR drop
        _visit(HashMap(i32, Vec(chr)), &h);
        #undef VISITOR
    }
    {
        _t(Res(bool, i32)) r = { .Ok = false };
        #define VISITOR debug
        _visit(Res(bool, i32), &r);
        #undef VISITOR
        printf("\n");
        _match(&r) {
            _arm(_pe(Res, Ok, _pb(x))) {
                printf("Res::Ok(%d)\n", *x);
            }
            _arm(_pe(Res, Err, _pb(x))) {
                printf("Res::Err(%d)\n", *x);
            }
        }
    }
}
