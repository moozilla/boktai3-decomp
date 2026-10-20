#include "global.h"

extern const s16 gUnk_086149C4[];

struct OrbVec {
    s16 x, y, z, pad;
};
struct OrbParticle {
    u32 flags;
    u8 unknown4[0x14];
    s16 x, y, z;
    u8 unknown1E[0xA];
    u8 active;
    u8 frame;
    u8 timer;
    u8 speed;
    u8 radius;
    u8 angle;
    u8 life;
    u8 unknown2F;
    struct OrbVec origin;
};
void SpriteDrawable_SelectTileFrame(void *, void *, u32);

static inline s32 Scale12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void ParticleBurst_UpdateOrbitParticle(struct OrbParticle *p, void *tiles)
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
        p->x = p->origin.x + Scale12(p->radius * gUnk_086149C4[(p->angle + 0x40) & 0xFF]);
        p->z = p->origin.z + Scale12(p->radius * gUnk_086149C4[p->angle]);
        p->y += p->speed;
    }
}
