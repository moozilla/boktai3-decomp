#include "global.h"
void Effect_Remove(void *);
u32 sub_08233900(u8 *s)
{
    u8 *p;
    s32 i;
    Effect_Remove(s + 0x74);
    p = s + 0xa4;
    i = 7;
    do {
        Effect_Remove(p);
        p += 0x2c;
        i--;
    } while (i >= 0);
    return 0;
}
