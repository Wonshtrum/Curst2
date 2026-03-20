#include <stdio.h>
#include "curst_std.h"
#include "curst_match.h"
#include "curst_fmt.h"

#define MyTuple(A, B) _struct(\
    MyTuple, (), (A, B),\
    _field(a, A),\
    _field(b, B))
#define _visit_MyTuple_debug(...) _derive(__VA_ARGS__)
#define _visit_MyTuple_fmt(...)    _derive(__VA_ARGS__)
#define _visit_MyTuple_eq(...)    _derive(__VA_ARGS__)

#define Tree(N, T) _struct(\
    Tree, (N), (T),\
    _field(val, T),\
    _field(children, Arr(N, Box(_rec(Tree, (N), (T))))))

#define User _struct(\
    User, (), (),\
    _field(name, String),\
    _field(age, u8),\
    _field(tag, i32))
#define _visit_User_debug(...) _derive(__VA_ARGS__)
#define _visit_User_clone(...) _derive(__VA_ARGS__)
#define _visit_User_drop(...)  _derive(__VA_ARGS__)
#define _visit_User_eq(...)    _derive(__VA_ARGS__)

#define Result(T, E) _enum(\
    Result, (), (T, E),\
    _variant(Ok, T),\
    _variant(Err, E),)
#define _visit_Result_debug(...) _derive(__VA_ARGS__)

#define Option(T) _enum(\
    Option, (), (T),\
    _variant(Some, T),\
    _variant(None, unit))
#define _visit_Option_debug(...) _derive(__VA_ARGS__)

#define VISITOR debug
_mono_with_alias(Arr(2, i32), my_alias)
_impl_with_alias(MyTuple(i32, i32), mt_debug, _This *this) {
    printf("MyTuple(%d, %d)", this->a, this->b);
}
_mono(User)
#undef VISITOR
#define VISITOR fmt
_mono(MyTuple(i32, i32))
#undef VISITOR

int main() {
    {
        _t(Vec(Vec(Vec(i32)))) t = {}, o = {};
        Vec_push(&t, Vec_new(Vec(i32)));
        Vec_push(&t.as_ptr[0], Vec_with_capacity(i32, 5));
        for (int i = 0; i < 10; i++) {
            Vec_push(&t.as_ptr[0].as_ptr[0], i);
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
        _t(Arr(2, Arr(3, i32))) t = {{ {{ 1, 2, 3 }}, {{ 4, 5, 6 }} }};
        #define VISITOR debug
        _visit(Arr(2, Arr(3, i32)), &t);
        #undef VISITOR
        printf("\n");
    }
    {
        _t(Arr(3, i32)) t = {{ 1, 2, 3 }};
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
        _t(User) u = { .name = String_from("Bertrand"), .age = 100 };
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
        let(h, HashMap_new(i32, char));
        for (int i = 0; i < 10; i++) {
            HashMap_insert(&h, i, '0'+i);
            #define VISITOR debug
            _visit(HashMap(i32, char), &h);
            #undef VISITOR
            printf("\n");
        }
        #define VISITOR clone
        let(h2, _visit(HashMap(i32, char), &h));
        #undef VISITOR
        printf("h2: ");
        #define VISITOR debug
        _visit(HashMap(i32, char), &h2);
        #undef VISITOR
        printf("\n");
        for (int i = 0; i < 12; i++) {
            _char *v = HashMap_get(&h, i);
            _char *v2 = HashMap_get(&h2, i);
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
        let(h, HashMap_new(i32, Vec(char)));
        for (int i = 0; i < 10; i++) {
            let(v, Vec_with_capacity(char, i));
            for (int j = 0; j < i; j++) {
                Vec_push(&v, '0'+j);
            }
        Vec_extend(&v, v);
            HashMap_insert(&h, i, v);
        }
        #define VISITOR debug
        _visit(HashMap(i32, Vec(char)), &h);
        #undef VISITOR
        printf("\n");
        HashMap_get(&h, 5)->as_ptr[3] = '@';
        for (int i = 0; i < 10; i++) {
            let(v, HashMap_get(&h, i));
            printf("h[%d]: ", i);
            #define VISITOR debug
            _visit(Vec(char), v);
            #undef VISITOR
            printf("\n");
        }
        #define VISITOR drop
        _visit(HashMap(i32, Vec(char)), &h);
        #undef VISITOR
    }
    {
        _t(Result(bool, i32)) r = { .Ok = false };
        #define VISITOR debug
        _visit(Result(bool, i32), &r);
        #undef VISITOR
        printf("\n");
        _match(&r) {
            _arm(_pe(Result, Ok, _pb(x))) {
                printf("Result::Ok(%d)\n", *x);
            }
            _arm(_pe(Result, Err, _pb(x))) {
                printf("Result::Err(%d)\n", *x);
            }
        }
    }
    {
        _t(Vec(Res(Opt(Res(i32, String)), i32))) v = {};
        Vec_push(&v, Res_Err(1337));
        Vec_push(&v, Res_Ok(Opt_None()));
        Vec_push(&v, Res_Ok(Opt_Some(Res_Err(String_from("unknown")))));
        Vec_push(&v, Res_Ok(Opt_Some(Res_Ok(42))));
        Vec_push(&v, Res_Ok(Opt_Some(Res_Ok(43))));
        for (_u32 i = 0; i < v.len; i++) {
            printf("element %d: ", i);
            _match(&v.as_ptr[i]) {
                _arm(_pe(Res, Err, _pb(e))) {
                    printf("top level error: %d\n", *e);
                }
                _arm(_pe(Res, Ok, _pe(Opt, Some, _pe(Res, Ok, _pb(x)_pc(*x == 42))))) {
                    printf("the universal answer\n");
                }
                _arm(_pe(Res, Ok, _pe(Opt, Some, _pe(Res, Err, _pb(msg))))) {
                    printf("error: \"%s\"\n", msg->as_ptr);
                }
                _arm(_pe(Res, Ok, _pe(Opt, Some, _pe(Res, Ok, _pb(x))))) {
                    printf("ok: %d\n", *x);
                }
                _arm(_pb(other)) {
                    printf("other: ");
                    #define VISITOR debug
                    _visit(Res(Opt(Res(i32, String)), i32), other);
                    #undef VISITOR
                    printf("\n");
                }
            }
        }
        printf("\n");
    }
    {
        _t(Arr(6, i32)) a = {{ 1, 2, 3, 4, 5, 6 }};
    // _foreach(i, in(a)) {
    //     printf("%d ", i);
    // }
    printf("\n");
        _t(Vec(i32)) v = {};
    let(s, _as_slice(&v));
    s = _as_slice(&a);
    printf("%d %d %d\n", sizeof(a), sizeof(v), sizeof(s));
    }

    let(hello, Str_new("hello"));
    let(s, String_new());
    Vec_extend(&s, hello);
    Vec_extend(&s, Str_new(" world"));
    Vec_push(&s, '!');
    #define VISITOR debug
    _visit(String, &s);
    #undef VISITOR
    Vec_push(&s, 0);
    printf("\n");
    printf("String { len: %d, cap: %d, ptr: \"%s\" }\n", s.len, s.cap, s.as_ptr);

    _t(MyTuple(i32, i32)) t = { .a = 42, .b = 1337 };
    _fat_ptr l1 = Fat_new(MyTuple(i32, i32), fmt, &t);
    let(dy, Dyn_new(MyTuple(i32, i32), (fmt, debug), &t));
    printf("===\n");
    Dyn_call(debug, &dy);
    Dyn_call(debug, &dy);
    Dyn_call(debug, &dy);
    _fat_ptr l2 = Dyn_into(debug, dy);
    Fat_call(debug, l2);
    printf("\n===\n");

    s.len--;
    String_writeln_fmt(&s, "--%?--", l1);
    println("==%?==", l1);
    Vec_push(&s, 0);
    printf("String { len: %d, cap: %d, ptr: \"%s\" }\n", s.len, s.cap, s.as_ptr);
}
