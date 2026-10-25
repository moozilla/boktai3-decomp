// MATCHING_CONCESSION: register-pinning
#include "global.h"

/* Tentative: emit a motion entry from a randomized point ahead or behind the actor. */

struct BurstVector {
    s16 x, y, z, unk_06;
};
struct ActorReaction {
    u8 unk_00[0x54];
    struct BurstVector position;
    u8 unk_5C[0x32B - 0x5C];
    u8 side;
};
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void MotionEntry_CreateWithDelta(struct BurstVector *, struct BurstVector *, s32, s32);
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline void Polar(struct BurstVector *out, s32 inputAngle, s32 radius)
{
    register s32 angle asm("r3") = inputAngle;
    out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void Add(struct BurstVector *out, struct BurstVector *a, struct BurstVector *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
static inline s32 RandomValue(void)
{
    u32 table = (u32)gUnk_0203B400;
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + table);
}
static inline s32 RandomMode(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + 0x0203B400);
}
#ifdef NONMATCHING
// fuzzy: 61.2%
// scope shape 141 (differences once low registers are set aside)
void Hresvelgr_EmitFacingMotion(struct ActorReaction *input)
{
    register struct ActorReaction *actor asm("r6") = input;
    struct ActorReaction *saved = actor;
    struct BurstVector position;
    struct BurstVector velocity;
    s32 sideSample = RandomValue() >> 3;
    s32 side = sideSample - sideSample / 2 * 2;
    s32 facing = actor->side;
    s32 sample = RandomValue() >> 3;
    {
        s32 quotient = sample / 1024 * 1024;
        struct BurstVector *out = &position;
        s32 angle = (side << 7) + (facing << 6) + 64;
        s32 radius = sample - quotient;
        out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
        out->y = 0;
        out->z = Scale(gUnk_086149C4[angle & 255] * radius);
    }
    Add(&position, &saved->position, &position);
    Polar(&velocity, actor->side << 6, 128);
    MotionEntry_CreateWithDelta(&position, &velocity, (RandomMode() >> 3) % 4, 32);
}
#else
INCLUDE_ASM("asm/nonmatching", Hresvelgr_EmitFacingMotion);
#endif
