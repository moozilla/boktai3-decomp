#include "global.h"

extern const s32 gUnk_0824FAB8[];
void memcpy(void *, const void *, u32);

s32 sub_081CE08C(u32 n)
{
    s32 t[40];
    s32 r;
    memcpy(t, gUnk_0824FAB8, 0xa0);
    if (n <= 0x27)
        r = t[n];
    else
        r = -1;
    return r;
}
