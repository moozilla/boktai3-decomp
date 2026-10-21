#include "global.h"
struct SpiralParticle {
    u8 unknown0[0x18];
    u16 x, y, z;
    u8 unknown1E[0xb];
    u8 angle, rise, accelerationFrame, animationFrame, angularSpeed;
    u8 unknown2E[2];
    u16 radius;
    u8 variant;
};
struct SpiralParticles {
    u8 unknown0[0x70];
    void *resource;
    u8 unknown74[0x3c0];
    u16 frame;
};
extern const s16 gUnk_086149C4[];
void SpriteDrawable_SelectTileFrame(struct SpiralParticle *, void *, u32);
void SpiralParticles_UpdateParticle(struct SpiralParticles *p, struct SpiralParticle *s)
{
    s32 delta;
    s32 product2;
    s32 product;
    s32 deltaZ;
    s->angle += s->angularSpeed;
    product2 = s->radius * gUnk_086149C4[(s->angle + 0x40) & 0xff];
    if (product2 >= 0)
        delta = product2 >> 12;
    else
        delta = -(-product2 >> 12);
    s->x += delta;
    product = s->radius * gUnk_086149C4[s->angle];
    if (product >= 0)
        deltaZ = product >> 12;
    else
        deltaZ = -(-product >> 12);
    s->z += deltaZ;
    if (p->frame >= s->accelerationFrame) {
        if (!(p->frame & 7) && s->angularSpeed > 2)
            s->angularSpeed--;
        s->radius += 6;
        s->y += s->rise;
    } else
        s->radius++;
    switch (s->variant) {
        case 0:
            if (s->animationFrame == 0)
                SpriteDrawable_SelectTileFrame(s, p->resource, 2);
            else if (s->animationFrame == 4)
                SpriteDrawable_SelectTileFrame(s, p->resource, 3);
            break;
        case 1:
            if (s->animationFrame == 0)
                SpriteDrawable_SelectTileFrame(s, p->resource, 4);
            else if (s->animationFrame == 4)
                SpriteDrawable_SelectTileFrame(s, p->resource, 5);
            break;
    }
    if (++s->animationFrame > 7)
        s->animationFrame = 0;
}
