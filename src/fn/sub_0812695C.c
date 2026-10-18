#include "global.h"
struct S {
    u8 f[0x18];
    u32 p;
    u8 g[0x25c - 0x1c];
    u32 c;
};
extern struct S *gUnk_02000158;
u32 AnimData_FindEntry(u32);
s32 BlinkingEffect_InitPool(struct S *s)
{
    s->p = AnimData_FindEntry(0x1c1a);
    s->c = 0;
    gUnk_02000158 = s;
    return 0;
}
