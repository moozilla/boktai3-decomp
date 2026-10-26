#include "global.h"
#include "area_effect.h"
s32 AreaEffect_HasEnabledTerrainValue(u32 a)
{
    u8 *g = gAreaEffectManager;
    u8 *np;
    s32 i;
    s32 n;
    u8 *e;
    if (g == 0)
        goto no;
    np = g + 0x26;
    if (*np != 0)
        goto go;
    goto no;
yes:
    return 1;
go:
    i = 0;
    n = *np;
    if (i >= n)
        goto no;
    np = (u8 *)n;
    e = g + 0xc6;
    do {
        if (*e != 0 && *(u16 *)(e + 4) == a)
            goto yes;
        e += 0xa8;
        i++;
    } while (i < (s32)np);
no:
    return 0;
}
