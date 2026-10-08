#include "global.h"
extern u8 *gUnk_0200010C;
s32 sub_0804DFC0(u32 a)
{
    u8 *g = gUnk_0200010C;
    u8 *np;
    s32 i;
    s32 n;
    u8 *e;
    if (g == 0) goto no;
    np = g + 0x26;
    if (*np != 0) goto go;
    goto no;
yes:
    return 1;
go:
    i = 0;
    n = *np;
    if (i >= n) goto no;
    np = (u8 *)n;
    e = g + 0xc6;
    do {
        if (*e != 0 && *(u16 *)(e + 4) == a) goto yes;
        e += 0xa8;
        i++;
    } while (i < (s32)np);
no:
    return 0;
}
