// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
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
struct GlyphTrail {
    u8 pad0[0x38];
    struct GlyphPosition position;
    u8 pad40[0x2E];
    u8 direction;
    u8 pad6F[8];
    u8 index;
    u8 pad78[4];
    struct GlyphSlot slots[4];
};
extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
static inline s32 RandomValue(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
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
// confidence=low: activates the next glyph with a random angular offset and radius.
#define ActorGlyphTrail_EmitParticle sub_08237A28
void ActorGlyphTrail_EmitParticle(struct GlyphTrail *p)
{
    s32 angle;
    register s32 radius asm("r2");
    const u16 *table;
    p->slots[p->index].active = 1;
    p->slots[p->index].flags &= ~1;
    p->slots[p->index].timer = 0;
    table = (const u16 *)RANDOM_TABLE_ADDR;
    p->slots[p->index].radius = ((RandomValue(table) >> 3) & 127) + 64;
    p->slots[p->index].angle = Mod(RandomValue(table), 96) - 48;
    angle = ((p->direction + 5) & 7) << 5;
    angle += WidenAngle(p->slots[p->index].angle);
    angle &= 255;
    p->slots[p->index].position = p->position;
    radius = p->slots[p->index].radius;
    p->slots[p->index].position.x += Truncate(gUnk_086149C4[(angle + 64) & 255] * radius);
    radius = p->slots[p->index].radius;
    p->slots[p->index].position.z += Truncate(gUnk_086149C4[angle] * radius);
    p->index++;
    if (p->index > 3)
        p->index = 0;
}
