#include "global.h"
#include "terrain_overlay.h"
void sub_08214514(struct TerrainOverlayDrawable *);
s32 TerrainOverlay_DestroyDrawables(struct TerrainOverlay *p)
{
    s32 i;
    i = 0;
    goto test;
loop: {
    struct TerrainOverlayDrawable *base = p->drawables;
    struct TerrainOverlayDrawable *d = (struct TerrainOverlayDrawable *)(sizeof(*base) * i + (u32)base);
    if (d->active)
        sub_08214514(d);
}
    i++;
test:
    if (i < p->drawableCount)
        goto loop;
    return 0;
}
