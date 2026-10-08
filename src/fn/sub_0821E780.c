#include "global.h"
struct E { u16 a; u16 b; u16 c; u16 d; };
struct O { u16 a; u16 b; u16 c; };
u16 sub_0821BD48(struct O *);
void sub_0821E780(struct O *o, struct E *t, u32 i)
{
    struct E *e = &t[i];
    o->a = e->a;
    o->c = e->b;
    o->b = sub_0821BD48(o);
}
