#include "player/effects/spiral_trail.h"
#include "engine/engine.h"
#include "scene/collision/contact_position.h"

void SpriteDrawable_SelectTileFrame(void *, void *, u32);

static inline s32 Scale12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void SpiralTrail_EmitOrbitParticle(struct SpiralTrailTask *owner,
                                   const struct ContactPosition *origin)
{
    struct SpiralTrailEmitter *emitter = &owner->emitter;

    if (++emitter->delay > 1) {
        struct SpiralTrailParticle *p = &emitter->particles[emitter->next];
        const u16 *table;
        u32 *seed;
        u16 index;
        u32 random;
        struct SpiralTrailParticle *sprite;

        p->active = 1;
        table = gRandomTable;
        seed = &gRandomIndex;
        index = *seed + 1;
        index &= 1023;
        random = table[index];
        p->speed = (random & 15) + 0x10;
        index = (index + 1) & 1023;
        random = table[index];
        p->radius = (random & 15) + 0x40;
        index = (index + 1) & 1023;
        *seed = index;
        p->angle = table[index];
        p->life = 8;
        p->timer = 0;
        p->origin = *origin;
        p->update = SpiralTrail_UpdateOrbitParticle;
        sprite = p;
        sprite->flags &= ~1;
        SpriteDrawable_SelectTileFrame(sprite, owner->emitter.graphics, sprite->frame);
        sprite->position.x =
            p->origin.x + Scale12(p->radius * gUnk_086149C4[(p->angle + 0x40) & 0xFF]);
        sprite->position.y = p->origin.y;
        sprite->position.z = p->origin.z + Scale12(p->radius * gUnk_086149C4[p->angle]);
        if ((++emitter->next) > 7) {
            emitter->next = 0;
        }
        emitter->delay = 0;
        owner->emitter.active = 1;
    }
}
