#include "display_animation.h"

u32 DisplayAnimation_GetTileIndex(struct DisplayAnimation *p)
{
    u8 *q;
    u32 mode = p->kind;
    if (mode == 0) {
        return 0;
    }
    if (mode == 1) {
        return p->resource.paletteIndex;
    }
    q = (u8 *)p + 0x2c;
    return *(u16 *)(q + 0x3a);
}
