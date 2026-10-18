#include "global.h"
struct FlickerParticle {
    u32 flags;
    u8 pad4[0x18 - 4];
    u16 x, y;
    u8 pad1c[0x28 - 0x1c];
    u16 age, active, velocityX, velocityY;
    u8 pad30[4];
    void *resource;
};
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void FlickerParticle_UpdateMotion(struct FlickerParticle *s)
{
    if (++s->age > 15) {
        s->flags |= 1;
        s->active = 0;
    } else {
        SpriteDrawable_SelectTileFrame(s, s->resource, 2 + ((s->age >> 2) & 1));
        s->x += s->velocityX;
        s->y += s->velocityY;
        if (!(s->age & 7))
            s->velocityY++;
    }
}
