#include "global.h"
void Effect_Remove(u8 *);
u32 sub_080189A8(u8 *p)
{
    s32 i;
    u8 *q = p + 0x3c;
    for (i = 7; i >= 0; i--) {
        Effect_Remove(q);
        q += 0x30;
    }
    return 0;
}
