#include "global.h"
extern u8 *gUnk_02000580;
extern u16 gUnk_02000534;
extern u16 gUnk_02000500;
s32 sub_080501AC(u8 *, s32);
s32 sub_08050310(u8 *p)
{
    u8 *g = gUnk_02000580;
    if (g[0x457] == 4) {
        u32 v = g[0x4a5];
        if (v == 1) {
            if (p[0x1c] & 0x10) goto yes;
        }
        if (v == 7) {
            if (p[0x1c] & 8) goto yes;
        }
        goto no;
    } else {
        u32 x;
        u32 w = gUnk_02000534;
        x = 0;
        if (w != 0) {
            if (gUnk_02000500 == 5) x = 1;
        }
        if (x == 0) {
            u32 t = gUnk_02000580[0x418];
            if (t == 3) goto call;
            if (t != 4) goto no;
        }
    call:
        if (sub_080501AC(p, 0x28) != 0) goto yes;
        goto no;
    }
yes:
    return 1;
no:
    return 0;
}
