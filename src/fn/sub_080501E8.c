#include "global.h"
extern u8 *gUnk_02000580;
extern u16 gUnk_02000534, gUnk_02000500;
u32 sub_080501AC(u8 *, s32);
u32 sub_080501E8(u8 *p)
{
    u8 *a = gUnk_02000580;
    u32 r;
    if (a[0x457] == 4) {
        u32 t = a[0x4A5];
        if (t == 1) {
            if (p[0x1c] & 0x10)
                goto yes;
        }
        if (t == 7) {
            if (p[0x1c] & 8)
                goto yes;
        }
        goto no;
    } else {
        u32 f, v;
        if (a[0x418] == 3)
            goto yes;
        v = gUnk_02000534;
        f = 0;
        if (v != 0) {
            if (gUnk_02000500 == 5)
                f = 1;
        }
        if (f == 0) {
            if (gUnk_02000580[0x418] != 4)
                goto no;
        }
        if (sub_080501AC(p, 0x28) == 0)
            goto no;
    }
yes:
    r = 1;
    goto end;
no:
    r = 0;
end:
    return r;
}
