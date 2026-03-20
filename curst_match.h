#include "do.h"

#define _m_bind(v, x) for(typeof(1?x:x)v=(x);__a;__a=0)
#define _m_shadow(v, x) _m_shadow_with(v,_##v,x)
#define _m_shadow_with(v, w, x) for(typeof(1?x:x)w=(x),v=w;__a;__a=0)

#define _match(x) for(int __m=1;__m;) for(typeof(1?x:x)__x=(x);__m;__m=0)
#define _arm(A) _arm##A

#define _pb(B) _m_bind(B, __x)
#define _pc(...) if(!(__VA_ARGS__)) __m=1; else
#define _pe(N, V, ...) if(__x->__tag != N##_##V) __m=1; else _pe_next(V, ##__VA_ARGS__, 1, 0, __VA_ARGS__)
#define _ps(...) DO(_ps_each, (__VA_ARGS__))
#define _pr(...) _m_bind(__x, *__x) __VA_ARGS__

#define _arm_guard(G) for(int __a=1;__m&&__a && (G);) for(__m=0;__a;__a=0)

#define _arm_pb(B) for(int __a=1;__m;__m=0) _pb(B)
#define _arm_pe(N, V, ...) _arm_guard(__x->__tag == N##_##V) _pe_next(V, ##__VA_ARGS__, 1, 0, __VA_ARGS__)
#define _arm_ps(...) _arm_guard(1) _ps(__VA_ARGS__)
#define _arm_pr(...) _arm_guard(1) _pr(__VA_ARGS__)

#define _pe_next(V, _, N, ...) _pe_next##N(V, __VA_ARGS__)
#define _pe_next0(V, _)
#define _pe_next1(V, _, next) _m_shadow(__x, &__x->V) next

#define _ps_each(i, F) _ps_each_ F
#define _ps_each_(F, ...) _ps_next(__COUNTER__, F, ##__VA_ARGS__, 1, 0, __VA_ARGS__)
#define _ps_next(C, F, _, N, ...) _ps_next##N(C, F, __VA_ARGS__)
#define _ps_next0(C, F, _) _m_bind(F, &__x->F)
#define _ps_next1(C, F, _, next) _m_bind(__x##C, __x) _m_shadow_with(__x,F,&__x->F) next _m_bind(__x, __x##C)
