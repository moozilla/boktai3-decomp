#include "global.h"
extern u8 *gUnk_020004D8;
void sub_0813A964(s32 i0)
{
    s32 i = i0;
    u8 *b;
    u8 *q;
    if (i >= 0) {
        b = gUnk_020004D8;
        if (b != 0) {
            i *= 0x64;
            q = b + 0x78;
            if (*(u32 *)(q + i) != 0) {
                u8 *r = b + 0x34;
                u32 *p = (u32 *)(r + i);
                *p = *p | 1;
            }
        }
    }
}
