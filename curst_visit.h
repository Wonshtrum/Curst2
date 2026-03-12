#ifndef __CURST_VISIT_H__
#define __CURST_VISIT_H__

// short circuit
#define _visitor_VISITOR(...)

#define _visitor_debug(...) __VA_ARGS__
#define _visitor_debug_args t
#define _visitor_debug_ret(Tt) void
#define _visitor_debug_sig(Tt) (Tt *t)
#define _forward_debug(V, _t) ({_shadow(t, _t); V;})

#define _visitor_eq(...) __VA_ARGS__
#define _visitor_eq_args t, o
#define _visitor_eq_ret(Tt) _bool
#define _visitor_eq_sig(Tt) (Tt *t, Tt *o)
#define _forward_eq(V, _t, _o) ({_shadow(t, _t); _shadow(o, _o); V;})

#define _visitor_clone(...) __VA_ARGS__
#define _visitor_clone_args t
#define _visitor_clone_ret(Tt) Tt
#define _visitor_clone_sig(Tt) (Tt *t)
#define _forward_clone(V, _t) ({_shadow(t, _t); V;})

#define _visitor_drop(...) __VA_ARGS__
#define _visitor_drop_args t
#define _visitor_drop_ret(Tt) void
#define _visitor_drop_sig(Tt) (Tt *t)
#define _forward_drop(V, _t) ({_shadow(t, _t); V;})

#define _visitor_hash_args t
#define _visitor_hash_ret(Tt) _u64
#define _visitor_sig(Tt) (Tt *t)


#endif // __CURST_VISIT_H__
