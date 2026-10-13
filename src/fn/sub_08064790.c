#include "global.h"

extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
int Mod(int, int);

static inline u32 tst(u8 *a, u32 m)
{
    return *a & m;
}

static inline u8 test(u8 *p, u32 m)
{
    if (tst(p, m))
        return TRUE;
    return FALSE;
}

void sub_08064790(u32 unused, u8 *p)
{
    u16 *q = (u16 *)(p + 0x60);
    if (*(s16 *)(p + 0x40) <= 0) {
        u32 one = 1;
        if (test(p + 0x36, one)) {
            u8 *f = p + 0x36;
            s32 m = -2;
            *f = *f & m;
        }
    } else {
        u32 one = 1;
        if (test(p + 0x36, one)) {
            int r;
            u32 v;
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
            r = Mod(*(u16 *)((gUnk_03005308 << 1) + (u32)gUnk_0203B400), 0x14);
            v = q[0xe];
            v -= 10;
            q[0xe] = v + r;
        }
        (*(u16 *)(p + 0x40))--;
    }
}
