// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 93.3%
// fix.py best (sweep after the inlined-insn fix)
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
    asm("" : "+r"(offset));
    return offset;
}
// confidence=low: advances four orbiting glyphs and hides each after fourteen frames.
#define ActorGlyphTrail_UpdateParticles sub_08237938
void ActorGlyphTrail_UpdateParticles(struct GlyphTrail *p)
{
    register s32 direction asm("r10") = ((p->direction + 5) & 7) << 5;
    register u16 *timer asm("r5") = &p->slots[0].timer;
    s32 i;
    typeof(timer = (u16 *)(((u8 *)timer) + 0x30)) t1;
    u32 k1 = 255;
    t1 = (timer = (u16 *)(((u8 *)timer) + 0x30));
    for (i = 0; i <= 3; i++, t1) {
        struct GlyphParticleView *slot = (void *)((((u8 *)p) + 0x94) + (i * 0x30));
        if (slot->active) {
            slot->timer++;
            if (slot->timer > 14) {
                slot->active = 0;
                p->slots[i].flags |= 1;
            } else {
                s32 radius = (slot->radius * (16 - (*timer))) >> 3;
                s32 angle = (direction + WidenAngle(slot->angle)) & k1;
                slot->position = p->position;
                slot->position.x += Truncate(gUnk_086149C4[(angle + 64) & k1] * radius);
                slot->position.z += Truncate(gUnk_086149C4[angle & k1] * radius);
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08237938);
#endif
