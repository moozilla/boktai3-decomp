#include "global.h"
#include "ram.h"
u32 sub_0812CE30(void)
{
    u8 *s = gFragmentEffectManager;
    s32 i;
    u8 *p;
    if (s == NULL)
        return 0;
    i = 0;
    p = s + 0xf6;
    for (; i <= 7; p += 0xc0, i++) {
        if ((u8)(*p - 1) <= 1)
            return 1;
    }
    return 0;
}
