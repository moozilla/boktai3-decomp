#include "global.h"

void Anim_Remove(u8 *);

u32 ModifierMenu_DestroyDisplayObjects(u8 *p)
{
    u8 *e = p + 0x17C;
    s32 i = 0x11;
    do {
        Anim_Remove(e);
        i--;
        e += 0x60;
    } while (i >= 0);
    return 0;
}
