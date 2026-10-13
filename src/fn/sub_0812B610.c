#include "global.h"
void Sprite_Remove(void *);
void sub_0812B610(u8 *p)
{
    u8 *q = p + 0x3c;
    s32 i = 1;
    do {
        Sprite_Remove(q);
        q += 0x5c;
    } while (--i >= 0);
}
