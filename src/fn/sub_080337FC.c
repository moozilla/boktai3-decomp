#include "global.h"
u8 *TextWindow_Find(u32);
void sub_08033AF8(void);
s32 sub_080337FC(u32 id)
{
    u32 *p = (u32 *)TextWindow_Find(id);
    s32 r;
    if (p) {
        p[0x5c / 4] = (u32)sub_08033AF8;
        r = 0;
    } else
        r = -1;
    return r;
}
