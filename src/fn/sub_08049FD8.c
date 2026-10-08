#include "global.h"
extern u8 *gUnk_02000488;
struct P { u32 a, b; };
u32 sub_08049FD8(struct P *a)
{
    u8 *p = gUnk_02000488;
    u32 r;
    if (p) {
        *a = *(struct P *)(p + 0x50);
        r = 1;
    } else r = 0;
    return r;
}
