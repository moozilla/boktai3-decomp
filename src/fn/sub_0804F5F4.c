#include "global.h"
extern u32 gUnk_02000580;
void sub_0804F448(u8 *);
void sub_0804F528(u8 *);
void sub_0804F568(u8 *);
void sub_0804F4AC(u8 *);
s32 sub_0804F5F4(u8 *p, u32 v)
{
    u32 g = gUnk_02000580;
    u32 z;
    u16 *h;
    *(u32 *)(p + 0x25c) = g;
    if (g == 0) return -1;
    h = (u16 *)(p + 0x260);
    z = 0;
    *h = v;
    sub_0804F448(p);
    sub_0804F528(p);
    sub_0804F568(p);
    sub_0804F4AC(p);
    p[0x262] = z;
    p[0x264] = z;
    p[0x265] = z;
    p[0x263] = z;
    if (*(u32 *)(p + 0x18) & 4)
        *(u16 *)(p + 0x266) = 7;
    else
        *(u16 *)(p + 0x266) = 1;
    return 0;
}
