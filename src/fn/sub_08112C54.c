#include "global.h"
extern u8 *gUnk_020004BC;
void sub_08112C54(s32 i)
{
    if (i >= 0) {
        u8 *g = gUnk_020004BC;
        if (g != 0) {
            s32 off = i * 0x60;
            u8 *q = g + 0x74;
            u8 *r;
            q = q + off;
            if (*(u32 *)q != 0) {
                r = g + 0x34;
                r = r + off;
                *(u32 *)r |= 1;
            }
        }
    }
}
