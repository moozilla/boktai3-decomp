#include "global.h"

struct S { u8 v; u8 f[0x2f]; u8 e[4][0x50]; };
void sub_08217EAC(void *);

u32 sub_0800A80C(u32 a, struct S *s)
{
    u8 (*e)[0x50] = s->e;
    s32 i;
    for (i = 3; i >= 0; i--) {
        sub_08217EAC(e);
        e++;
    }
    s->v = 0;
}
