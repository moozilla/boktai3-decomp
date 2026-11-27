#include "global.h"

void Anim_Remove(u8 *);

void LinkScorePanel_RemoveSprites(u8 *p)
{
    u8 *q = p + 0x24;
    s32 i;

    for (i = 0x11; i >= 0; i--) {
        Anim_Remove(q);
        q += 0x60;
    }
}
