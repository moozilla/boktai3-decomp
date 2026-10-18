#include "global.h"

void Anim_Remove(u8 *);

void Registry3C9F_RemoveAnimations(u8 *p)
{
    u8 *q = p + 0x20;
    s32 i;

    for (i = 9; i >= 0; i--) {
        Anim_Remove(q);
        q += 0x60;
    }
}
