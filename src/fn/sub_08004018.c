#include "global.h"
struct Drawable {
    u32 flags;
    u8 active;
    u8 pad[0x2c - 5];
};
struct Manager {
    u8 pad[0x24];
    s32 drawableCount;
    u8 pad28[0x88 - 0x28];
    struct Drawable *drawables;
};
void sub_08214514(struct Drawable *);
s32 TerrainOverlay_DestroyDrawables(struct Manager *p)
{
    s32 i;
    i = 0;
    goto test;
loop: {
    struct Drawable *base = p->drawables;
    struct Drawable *d = (struct Drawable *)(sizeof(*base) * i + (u32)base);
    if (d->active)
        sub_08214514(d);
}
    i++;
test:
    if (i < p->drawableCount)
        goto loop;
    return 0;
}
