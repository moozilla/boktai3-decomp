#include "global.h"

struct A {
    u8 f[0x55c];
    u32 a;
    u32 b;
};

void Registry2388_SetUpdate(struct A *p, u32 v)
{
    p->a = v;
    p->b = 0;
}
