#include "display_animation.h"

s32 DisplayAnimation_SetTileSource(struct DisplayAnimation *p, u32 v)
{
    u32 mode = p->kind;
    if (mode == 0) {
        return -1;
    }
    if (mode == 1) {
        *(u32 *)((u8 *)p + 0x18) = v;
    } else {
        *(u32 *)((u8 *)p + 0x74) = v;
    }
    return 0;
}
