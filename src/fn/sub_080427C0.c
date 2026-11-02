#include "display_animation.h"

s32 DisplayAnimation_SetTileSource(struct DisplayAnimation *p, u32 v)
{
    u32 mode = p->kind;
    if (mode == 0) {
        return -1;
    }
    if (mode == 1) {
        p->resource.paletteData = (const void *)v;
    } else {
        p->keyframeTileSource = (const void *)v;
    }
    return 0;
}
