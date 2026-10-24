#include "global.h"

// confidence=low: sub_08236DC4's trail setup for twelve 0x44-byte slots; frame 2 for kind 0, else 4.
struct TrailSlot {
    u8 pad0[0xF];
    u8 f0F;
    u8 f10;
    u8 pad11[0x17];
    u8 f28;
    u8 f29;
    u8 pad2A[0x1A];
};
struct TrailParticles {
    u8 state;
    u8 state1;
    u8 pad2[2];
    void *resource;
    struct TrailSlot slots[12];
};
struct Trail {
    u8 pad0[0x18];
    u8 kind;
    u8 pad19[0x138 - 0x19];
    struct TrailParticles particles;
};
void *AnimData_FindEntry(s32);
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, u32);

void sub_082364A4(struct Trail *p)
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
    if (p->kind == 0)
        frame = 2;
    else
        frame = 4;
    offset = -4;
    f = &q->slots[0].f0F;
    slot = q->slots;
    i = 11;
    do {
        f[0x19] = 0;
        f[0x1A] = frame;
        ParticleSprite_Initialize(slot, q->resource, 1);
        Effect_SetOrigin(slot, offset, offset);
        Effect_SetPalette(slot, 1);
        f[1] = 0xF8;
        f[0] = 2;
        f += 0x44;
        slot++;
        i--;
    } while (i >= 0);
}
