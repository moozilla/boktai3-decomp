#include "player/effects/spiral_trail.h"

extern const s16 gUnk_086149C4[];

void SpriteDrawable_SelectTileFrame(void *, void *, u32);

static inline s32 Scale12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void SpiralTrail_UpdateOrbitParticle(struct SpiralTrailParticle *p, void *tiles)
{
    if (++p->timer > 29) {
        p->flags |= 1;
        p->active = 0;
    } else {
        if (p->timer == 6)
            SpriteDrawable_SelectTileFrame(p, tiles, p->frame + 1);
        if (p->timer > 3) {
            p->speed++;
            if (p->timer > 8)
                p->life--;
        }
        p->angle += p->life;
        p->position.x = p->origin.x + Scale12(p->radius * gUnk_086149C4[(p->angle + 0x40) & 0xFF]);
        p->position.z = p->origin.z + Scale12(p->radius * gUnk_086149C4[p->angle]);
        p->position.y += p->speed;
    }
}
