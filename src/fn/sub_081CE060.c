#include "global.h"

extern const s32 gUnk_0824FA18[];
void sub_0824DA48(void *, const void *, u32);

s32 sub_081CE060(u32 n)
{
    s32 t[40];
    s32 r;
    sub_0824DA48(t, gUnk_0824FA18, 0xa0);
    if (n <= 0x27) r = t[n];
    else r = -1;
    return r;
}
