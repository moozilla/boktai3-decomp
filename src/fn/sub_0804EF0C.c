#include "global.h"
#define AreaEffect_InitializeLink sub_0804EF0C

struct S {
    u8 filler[0x24];
    u16 a;
    u8 b;
    u8 c;
    u8 d;
};
extern struct S *gUnk_0200010C;

s32 AreaEffect_InitializeLink(struct S *p)
{
    p->b = 0;
    p->a = 0x1C7;
    p->d = 0;
    gUnk_0200010C = p;
    return 0;
}
