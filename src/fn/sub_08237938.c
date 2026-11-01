#include "global.h"
struct GlyphPosition {
    u16 x, y, z, pad;
} __attribute__((aligned(4)));
struct GlyphSlot {
    u32 flags;
    u8 pad4[0x14];
    struct GlyphPosition position;
    u8 pad20[8];
    u8 active;
    s8 angle;
    u16 radius, timer, pad2E;
};
struct GlyphParticleView {
    struct GlyphPosition position;
    u8 pad8[8];
    u8 active;
    s8 angle;
    u16 radius, timer;
};
struct GlyphTrail {
    u8 pad0[0x38];
    struct GlyphPosition position;
    u8 pad40[0x2E];
    u8 direction;
    u8 pad6F[0xD];
    struct GlyphSlot slots[4];
};
extern const s16 gUnk_086149C4[];
static inline s32 Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline s32 WidenAngle(s32 offset)
{
    offset += 256;
    return offset;
}
// confidence=low: advances four orbiting glyphs and hides each after fourteen frames.
void ActorGlyphTrail_UpdateParticles(struct GlyphTrail *p)
{
    s32 direction = ((p->direction + 5) & 7) << 5;
    u32 mask = 255;
    const s16 *sines = gUnk_086149C4;
    s32 offset = 0;
    struct GlyphParticleView *slot = (void *)&p->slots[0].position;
    u16 *timer = &p->slots[0].timer;
    s32 count = 3;
    do {
        if (slot->active) {
            slot->timer++;
            if (slot->timer > 14) {
                slot->active = 0;
                {
                    struct GlyphSlot *base = p->slots;
                    u32 *flags = (u32 *)(offset + (u8 *)base);
                    *flags |= 1;
                }
            } else {
                s32 radius = (slot->radius * (16 - (*timer))) >> 3;
                s32 angle = (direction + WidenAngle(slot->angle)) & mask;
                slot->position = p->position;
                slot->position.x += Truncate(sines[(angle + 64) & mask] * radius);
                slot->position.z += Truncate(sines[angle & mask] * radius);
            }
        }
        offset += sizeof(struct GlyphSlot);
        slot = (void *)((u8 *)slot + sizeof(struct GlyphSlot));
        timer = (void *)((u8 *)timer + sizeof(struct GlyphSlot));
    } while (--count >= 0);
}
