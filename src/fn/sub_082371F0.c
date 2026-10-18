#include "global.h"
struct RisingSprite {
    u32 flags;
    u8 unknown4[0x16];
    u16 height;
    u8 unknown1C[0xC];
    void *resource;
    u8 active, unknown2D;
    u16 frame;
};
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void RisingSprite_Update(struct RisingSprite *p)
{
    SpriteDrawable_SelectTileFrame(p, p->resource, (p->frame >> 1) + 12);
    p->height += 10;
    p->frame++;
    if (p->frame == 16) {
        p->flags |= 1;
        p->active = 0;
    }
}
