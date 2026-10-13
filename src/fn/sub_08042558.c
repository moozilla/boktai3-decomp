#include "global.h"

u32 Sprite_Remove(u8 *);
u32 Anim_Remove(u8 *);

s32 sub_08042558(u8 *p)
{
    u32 mode = *(u32 *)p;
    if (mode == 0) {
        return -1;
    }
    if (mode == 1) {
        Sprite_Remove(p + 0x28);
    } else {
        Anim_Remove(p + 0x2c);
    }
    *(u32 *)p = 0;
    return 0;
}
