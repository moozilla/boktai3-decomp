#include "global.h"
struct EffectVector {
    s16 x, y, z, pad;
};
struct EffectCoordinates {
    u8 unknown0[8];
    struct EffectVector position;
};
static inline s32 HalfTowardZero(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -((-value) >> 1);
}
void PositionedEffect_SetHalfScaleCoordinates(struct EffectCoordinates *out,
                                              struct EffectVector *source)
{
    out->position.x = HalfTowardZero(source->x - 640) + 512;
    out->position.z = HalfTowardZero(source->z - 640) + 512;
}
