#include "global.h"
#include "ram.h"
void sub_0812CE64(void)
{
    u8 *s = gFragmentEffectManager;
    s32 i;
    if (s) {
        u8 v = 3;
        s += 0xf6;
        for (i = 7; i >= 0; s += 0xc0, i--) {
            if (*s != 3 && *s != 0)
                *s = v;
        }
    }
}
