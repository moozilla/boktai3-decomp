#include "global.h"
#include "engine.h"
#include "ram.h"

extern const s16 gUnk_086149C4[];

struct Trail080AA744Osc {
    u8 unknown0[0xE9];
    u8 flags;
    u8 unknownEA[0x16];
    u16 offsetX;
    u16 offsetY;
    u16 offsetZ;
    u8 unknown106[0x22];
};

struct Trail080AA744 {
    u8 unknown0[0x3F0];
    struct Trail080AA744Osc segments[3];
    u8 unknown768[0x168];
    u16 flags8D0;
    u8 unknown8D2[8];
    s16 segmentCount;
};

struct Actor080AA744 {
    u8 unknown0[0x2EC];
    s16 velocity[3];
    u8 unknown2F2[0x3D0 - 0x2F2];
    struct Trail080AA744 *trail;
};

static inline u8 Trail080AA744IsActive(struct Trail080AA744Osc *seg)
{
    u32 value = seg->flags;
    u32 mask = 2;
    if (mask & value)
        return 1;
    return 0;
}

static inline u8 Trail080AA744IsHidden(struct Trail080AA744Osc *seg)
{
    u32 value = seg->flags;
    u32 mask = 1;
    if (mask & value)
        return 1;
    return 0;
}

static inline s32 Trail080AA744FixedMul(s32 a, s32 b)
{
    s32 product = a * b;
    if (product >= 0)
        return product >> 12;
    return -((-product) >> 12);
}

#ifdef NONMATCHING
// fuzzy: 35.4%
// scope shape 109 (differences once low registers are set aside)
// confidence=low: sets each active trail oscillator's X and Z offsets from a random-table angle, scaled by velocity magnitude; raises a sound flag once
void sub_080AA744(struct Actor080AA744 *actor)
{
    struct Trail080AA744 *trail = actor->trail;
    s32 mask;
    s32 i;
    s32 magSq = actor->velocity[0] * actor->velocity[0] + actor->velocity[1] * actor->velocity[1] +
                actor->velocity[2] * actor->velocity[2];

    if (magSq > 0x400)
        mask = 0xFF;
    else if (magSq > 0x80)
        mask = 0x7F;
    else
        mask = 0x3F;

    for (i = 0; i < trail->segmentCount; i++) {
        struct Trail080AA744Osc *seg = &trail->segments[i];
        if (Trail080AA744IsActive(seg) && !Trail080AA744IsHidden(seg)) {
            u16 r;
            gRandomIndex = (gRandomIndex + 1) & 0x3FF;
            r = gRandomTable[gRandomIndex];
            r &= mask;
            seg->offsetX = Trail080AA744FixedMul(gUnk_086149C4[(r + 0x40) & 0xFF], 12);
            seg->offsetY = 0;
            seg->offsetZ = Trail080AA744FixedMul(gUnk_086149C4[r & 0xFF], 12);
        }
    }

    if (trail->flags8D0 & 4) {
        Sound_PlaySong(0x3D4);
        trail->flags8D0 &= ~4;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080AA744);
#endif
