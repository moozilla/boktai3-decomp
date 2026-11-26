#include "global.h"

struct Pos {
    u32 a, b;
};

struct Particle {
    u32 flags;
    u8 pad4[0x14];
    struct Pos pos;
    u8 pad20[8];
    struct Pos vel;
    u8 active;
    u8 age;
    u8 frame;
    u8 pad33;
};

struct EffectState {
    u8 pad0[0x868];
    void *resource;
    u8 active;
    u8 cursor;
    u8 pad_a[2];
    struct Particle entries[6];
};

void SpriteDrawable_SelectTileFrame(void *, void *, s32);

void DirectionalEffect_AddParticle(struct EffectState *p, struct Pos *pos, struct Pos *vel,
                                   u32 lifetime)
{
    struct Particle *e = &p->entries[p->cursor];
    u8 *cursor = &p->cursor;
    e->flags &= ~1;
    e->frame = lifetime * 2 + 2;
    SpriteDrawable_SelectTileFrame(e, p->resource, e->frame);
    e->pos = *pos;
    e->vel = *vel;
    e->age = 0;
    e->active = 1;
    p->active = 1;
    (*cursor)++;
    if (*cursor > 5)
        *cursor = 0;
}
