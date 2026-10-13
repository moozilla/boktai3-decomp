#include "global.h"
void Effect_Remove(u8 *);
u32 sub_080569D8(u8 *p)
{
    s32 i = 0;
    if (i < *(u16 *)(p + 0x1C)) {
        u8 *q = p + 0x20;
        do {
            Effect_Remove(q);
            q += 0x28;
            i++;
        } while (i < *(u16 *)(p + 0x1C));
    }
    return 0;
}
