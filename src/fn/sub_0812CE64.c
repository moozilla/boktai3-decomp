#include "global.h"
extern u8 *gUnk_0200018C;
void sub_0812CE64(void)
{
    u8 *s = gUnk_0200018C;
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
