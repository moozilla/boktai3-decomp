#include "global.h"
#define gEffectSineTable gUnk_086149C4
struct EffectPosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern const s16 gUnk_086149C4[];
void sub_081805F0(void *, s32, struct EffectPosition *);
static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void ActorEffect_SpawnAtAngle(void *actor, struct EffectPosition *origin, s32 angle)
{
    struct EffectPosition position = *origin;
    angle &= 255;
    position.x += scale(170 * gUnk_086149C4[(angle + 64) & 255]);
    position.y += 230;
    position.z += scale(170 * gUnk_086149C4[angle]);
    sub_081805F0(actor, angle, &position);
}
