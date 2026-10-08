#include "global.h"
extern u8 *gUnk_02000710;
s32 sub_081BDA10(u32 a, s32 v)
{
    s32 i;
    s16 *q;
    s32 r;
    if (v <= 0xf) goto setup;
    goto none;
found:
    r = i;
    goto end;
setup:
    i = 0;
    q = (s16 *)(gUnk_02000710 + 0x68);
    do {
        if (*q == v) goto found;
        q++;
        i++;
    } while (i <= 7);
none:
    r = -1;
end:
    return r;
}
