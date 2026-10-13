#include "global.h"
void Effect_Remove(void *);
void sub_08236484(u8 *s)
{
    u8 *p = s + 0x140;
    s32 i = 11;
    do {
        Effect_Remove(p);
        p += 0x44;
        i--;
    } while (i >= 0);
}
