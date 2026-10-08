#include "global.h"

extern u32 gUnk_02000074;
void sub_08046F08(u32, s32);
u32 sub_080471A8(u32 p)
{
    s32 i = 0;
    do {
        sub_08046F08(p, i);
        i++;
    } while (i <= 3);
    return gUnk_02000074 = 0;
}
