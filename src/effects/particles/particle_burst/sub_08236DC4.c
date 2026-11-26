#include "global.h"

struct TrailSlot {
    u8 pad0[0xF];
    u8 f0F;
    u8 f10;
    u8 pad11[0x17];
    u8 f28;
    u8 f29;
    u8 pad2A[0x12];
};
struct TrailParticles {
    u8 state;
    u8 state1;
    u8 pad2[2];
    void *resource;
    struct TrailSlot slots[8];
};
struct Trail {
    u8 pad0[0x24];
    u8 kind;
    u8 pad25[0x9C - 0x25];
    struct TrailParticles particles;
};
void *AnimData_FindEntry(s32);
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, u32);

void sub_08236DC4(struct Trail *p)
{
    struct TrailParticles *q = &p->particles;
    struct TrailSlot *slot;
    u8 *f;
    s32 i;
    s32 frame;
    s32 offset;
    q->resource = AnimData_FindEntry(0x1C1E);
    q->state = 0;
    q->state1 = 0;
    if (p->kind == 7)
        frame = 14;
    else
        frame = (p->kind + 1) * 2;
    offset = -4;
    f = &q->slots[0].f0F;
    slot = q->slots;
    i = 7;
    do {
        f[0x19] = 0;
        f[0x1A] = frame;
        ParticleSprite_Initialize(slot, q->resource, 1);
        Effect_SetOrigin(slot, offset, offset);
        Effect_SetPalette(slot, 1);
        f[1] = 0xF8;
        f[0] = 2;
        f += 0x3C;
        slot++;
        i--;
    } while (i >= 0);
}
