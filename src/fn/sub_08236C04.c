#include "global.h"
#include "engine.h"

struct Unk08236C04Position {
    s16 x, y, z, pad;
};
struct Unk08236C04Particle {
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
    struct Unk08236C04Position origin;
    void *update;
};
struct Unk08236C04Emitter {
    u8 next, delay, spawned, unknown3;
    void *graphics;
    struct Unk08236C04Particle particles[8];
};
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void ParticleBurst_UpdateOrbitParticle(void);

static inline s32 Scale12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void ParticleBurst_EmitOrbitParticle(u8 *owner, struct Unk08236C04Position *origin)
{
    struct Unk08236C04Emitter *emitter = (struct Unk08236C04Emitter *)(owner + 0x9C);

    if (++emitter->delay > 1) {
        struct Unk08236C04Particle *p = &emitter->particles[emitter->next];
        const u16 *table;
        u32 *seed;
        unsigned short i;
        u32 mask;
        struct Unk08236C04Particle *sprite;

        p->active = 1;
        table = gRandomTable;
        seed = &gRandomIndex;
        i = *seed + 1;
        mask = 1023;
        i &= mask;
        {
            u32 r = table[i];
            p->speed = (r & 15) + 0x10;
        }
        i = (i + 1) & mask;
        {
            u32 r = table[i];
            p->radius = (r & 15) + 0x40;
        }
        i = (i + 1) & mask;
        *seed = i;
        p->angle = table[i];
        p->life = 8;
        p->timer = 0;
        p->origin = *origin;
        p->update = ParticleBurst_UpdateOrbitParticle;
        sprite = p;
        sprite->flags &= ~1;
        SpriteDrawable_SelectTileFrame(sprite, *(void **)(owner + 0xA0), sprite->frame);
        sprite->x = p->origin.x + Scale12(p->radius * gUnk_086149C4[(p->angle + 0x40) & 0xFF]);
        sprite->y = p->origin.y;
        sprite->z = p->origin.z + Scale12(p->radius * gUnk_086149C4[p->angle]);
        if ((++emitter->next) > 7) {
            emitter->next = 0;
        }
        emitter->delay = 0;
        owner[0x9E] = 1;
    }
}
