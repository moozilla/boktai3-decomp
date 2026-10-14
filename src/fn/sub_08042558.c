#include "display_animation.h"

s32 DisplayAnimation_Destroy(struct DisplayAnimation *p)
{
    u32 mode = p->kind;
    if (mode == 0) {
        return -1;
    }
    if (mode == 1) {
        Sprite_Remove((u8 *)p + 0x28);
    } else {
        Anim_Remove((u8 *)p + 0x2c);
    }
    p->kind = 0;
    return 0;
}
