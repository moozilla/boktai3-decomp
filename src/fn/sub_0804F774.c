#include "global.h"
extern u8 *gUnk_02000110;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 sub_0804F774(void)
{
    u8 **g = &gUnk_02000110;
    u32 v;
    s32 i;
    s32 n;
    s32 m;
    u8 *o;
    u8 *a;
    u8 *b;
    if (*g == 0) goto fail;
    if (Script_SeekToKeyword(0x6e) != 0) goto go;
    goto fail;
found:
    return *(u8 *)(b + 0x100);
go:
    v = Script_GetValue();
    i = 0;
    o = *g;
    n = *(u16 *)(o + 0x18);
    if (i >= n) goto fail;
    m = n;
    a = o + 0x104;
    b = o;
    do {
        if (*(u16 *)a == v) goto found;
        a += 0xfc;
        b += 0xfc;
        i++;
    } while (i < m);
fail:
    return -1;
}
