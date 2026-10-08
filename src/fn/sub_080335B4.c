#include "global.h"

extern u32 gUnk_020000E0;
void sub_08033284(void);
s32 sub_080335B4(void)
{
    s32 r;
    if (gUnk_020000E0 != 0) {
        sub_08033284();
        r = 0;
    } else
        r = -1;
    return r;
}
