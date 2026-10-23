// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct GlyphPosition {
    u16 x, y, z, pad;
} __attribute__((aligned(4)));
struct GlyphOwner {
    u8 pad0[0x30];
    struct GlyphPosition position;
    u8 pad38[0x36B];
    u8 side, direction;
};
struct GlyphTrail {
    u8 pad0[0x18];
    struct GlyphOwner *owner;
    u8 pad1C[0x1C];
    struct GlyphPosition position;
    u8 pad40[0x24];
    u16 motionX, motionY, motionZ, pad6A, speed;
    u8 direction;
};
extern const s16 gUnk_086149C4[];
static inline s32 Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline s32 Primary(s32 value)
{
    s32 result = Truncate(value);
    return result;
}
// confidence=low: offsets a trail anchor to either side of its owner and sets its velocity.
#define ActorGlyphTrail_UpdateAnchor sub_082377B8
void ActorGlyphTrail_UpdateAnchor(struct GlyphTrail *p)
{
    struct GlyphOwner *owner = p->owner;
    s32 angle, cosine, sine;
    p->position = owner->position;
    p->position.y += 200;
    angle = ((owner->direction + 5) & 7) << 5;
    cosine = gUnk_086149C4[(angle + 64) & 255];
    sine = gUnk_086149C4[angle & 255];
    if (owner->side) {
        p->position.x += Primary(cosine * 170) + Truncate(sine * 8);
        p->position.z += Primary(sine * 170) - Truncate(cosine * 8);
    } else {
        p->position.x += Primary(cosine * 170) - Truncate(sine * 8);
        p->position.z += Primary(sine * 170) + Truncate(cosine * 8);
    }
    {
        s32 direction = (angle + 112) >> 5;
        u32 mask = 7;
        asm("" : "+r"(mask));
        p->direction = mask & direction;
    }
    p->motionX = Truncate(gUnk_086149C4[(angle + 64) & 255] * p->speed);
    p->motionZ = Truncate(gUnk_086149C4[angle] * p->speed);
}
