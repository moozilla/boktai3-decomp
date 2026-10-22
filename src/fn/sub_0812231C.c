#include "global.h"
struct EffectSourceObject;
struct EffectMotion;
s32 SpriteEffect_SpawnWithMotion(struct EffectSourceObject *, struct EffectSourceObject *,
                                 struct EffectMotion *, u32, u32, u32, u32);

s32 sub_0812231C(struct EffectSourceObject *source, struct EffectSourceObject *target,
                 struct EffectMotion *motion, u32 a, u32 b, u32 c, u32 d)
{
    return SpriteEffect_SpawnWithMotion(source, target, motion, a, b, c, d);
}
