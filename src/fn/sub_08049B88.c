#include "global.h"
extern u8 *gUnk_02000488;
struct P { u32 a, b; };
u32 sub_08049B88(struct P *a)
{
    u8 *p = gUnk_02000488;
    u32 r;
    if (p) {
        *a = *(struct P *)(p + 0x474);
        r = 1;
    } else {
    ((u16 *)a)[0] = 0;
    ((u16 *)a)[1] = 0;
    ((u16 *)a)[2] = 0;
    r = 0;
    }
    return r;
}
