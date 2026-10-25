#include "contact_actor.h"

// confidence=low: spawns one trail particle at the position with a random heading and speed.

extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
u32 SceneMovingParticle_CreateWithOptions(void *, void *, s32, s32, s32);

static inline s32 Trunc080DF908(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

#ifdef NONMATCHING
// fuzzy: 49.5%
// scope shape 30 (differences once low registers are set aside)
// confidence=low: spawns one trail particle at the position with a random heading and speed.
void SceneActorAnimation_SpawnTrailAtPosition(void *arg0, u32 arg1)
{
    s16 velocity[3];
    s16 *v = velocity;
    u32 *cursor = &gUnk_03005308;
    u32 index;
    u32 mask;
    s32 angle;
    s32 magnitude;

    index = (*cursor + 1) & 1023;
    mask = 255;
    angle = *(const u8 *)(index * 2 + (u32)gUnk_0203B400);
    index = (index + 1) & 1023;
    *cursor = index;
    {
        s32 raw = gUnk_0203B400[index];
        s32 bitmask = 31;
        raw &= bitmask;
        magnitude = raw;
    }
    v[0] = Trunc080DF908(gUnk_086149C4[(angle + 64) & mask] * magnitude);
    v[1] = 0;
    v[2] = Trunc080DF908(gUnk_086149C4[angle] * magnitude);
    *cursor = (*cursor + 1) & 1023;
    v[1] = (gUnk_0203B400[*cursor] & 63) - 48;
    if (arg1 == 3)
        SceneMovingParticle_CreateWithOptions(arg0, &velocity, 60, 3, 3);
    else
        SceneMovingParticle_CreateWithOptions(arg0, &velocity, 60, 4, 3);
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorAnimation_SpawnTrailAtPosition);
#endif
