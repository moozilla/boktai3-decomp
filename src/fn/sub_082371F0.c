#include "global.h"
#define RisingSprite_Update sub_082371F0
struct RisingSprite {
    u32 flags;
    u8 unknown4[0x16];
    u16 height;
    u8 unknown1C[0xC];
    void *resource;
    u8 active, unknown2D;
    u16 frame;
};
void sub_08217EEC(void *, void *, u32);
void sub_082371F0(struct RisingSprite *p)
{
    sub_08217EEC(p, p->resource, (p->frame >> 1) + 12);
    p->height += 10;
    p->frame++;
    if (p->frame == 16) {
        p->flags |= 1;
        p->active = 0;
    }
}
