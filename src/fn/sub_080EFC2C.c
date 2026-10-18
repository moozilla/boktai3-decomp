#include "global.h"

struct T {
    u8 f[6];
    u16 flags;
};
struct S {
    u8 filler[0x188];
    struct T t;
};

s32 Ivy_SetAuxiliaryColliderFlag(struct S *s)
{
    struct T *t = &s->t;
    u32 m = 4;
    t->flags = m | t->flags;
    return 1;
}
