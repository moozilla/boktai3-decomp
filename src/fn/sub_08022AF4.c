#include "global.h"

struct S {
    u8 f[0x3c0];
    u32 b;
    u8 g[0x9dc - 0x3c4];
    u32 a;
};
void BackgroundFlash_BeginDisappearance(u32);
void sub_08022AF4(struct S *p)
{
    BackgroundFlash_BeginDisappearance(p->a);
    p->b = 0;
}
