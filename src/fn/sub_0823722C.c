#include "global.h"
void RisingSprite_Update(void *);
u32 RisingPuffPool_UpdateTask(u8 *s)
{
    u8 *p = s + 0x18;
    u8 *q = s + 0x44;
    s32 i = 7;
    do {
        if (*q != 0)
            RisingSprite_Update(p);
        p += 0x30;
        q += 0x30;
        i--;
    } while (i >= 0);
    return 0;
}
