#include "global.h"

void Anim_Remove(u8 *);
void Effect_Remove(u8 *);

s32 sub_0815FFB8(u8 *p)
{
    u8 *q;
    s32 i;
    Anim_Remove(p + 0x3c);
    q = p + 0xfc;
    i = 15;
    do {
        Effect_Remove(q);
        q += 0x3c;
        i--;
    } while (i >= 0);
    return 0;
}
