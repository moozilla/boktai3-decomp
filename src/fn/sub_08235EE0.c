#include "global.h"
#include "engine.h"

struct SpPosition {
    s16 x, y, z, pad;
};
struct SpParticle {
    u32 flags;
    u8 unknown4[0xB];
    u8 visible;
    u8 unknown10[8];
    s16 x, y, z;
    u8 unknown1E[0xA];
    u8 active;
    u8 frame;
    u8 angle;
    u8 life;
    u16 timer;
    u16 speed;
    u16 radius;
    u8 pad32[6];
    struct SpPosition origin;
    void *update;
};
struct SpEmitter {
    u8 next, delay, spawned, unknown3;
    void *graphics;
    struct SpParticle particles[12];
};
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void sub_08235E20(void);

static inline s32 Scale12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void CallbackGroup_EmitAtPosition(u8 *owner, struct SpPosition *origin)
{
    struct SpEmitter *emitter = (struct SpEmitter *)(owner + 0x138);

    if (++emitter->delay > 1) {
        struct SpParticle *p = &emitter->particles[emitter->next];
        const u16 *table;
        u32 *seed;
        u32 i;
        u32 mask;
        struct SpParticle *sprite;

        p->active = 1;
        table = gRandomTable;
        seed = &gRandomIndex;
        *(&i) = (*seed) + 1;
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
        p->update = sub_08235E20;
        sprite = p;
        sprite->flags &= ~1;
        SpriteDrawable_SelectTileFrame(sprite, *(void **)(owner + 0x13C), sprite->frame);
        sprite->x = p->origin.x + Scale12(sprite->radius * gUnk_086149C4[(p->angle + 0x40) & 0xFF]);
        sprite->y = p->origin.y;
        sprite->z = p->origin.z + Scale12(p->radius * gUnk_086149C4[p->angle]);
        sprite->visible = 2;
        if ((++emitter->next) > 11) {
            emitter->next = 0;
        }
        emitter->delay = 0;
        owner[0x13A] = 1;
    }
}
