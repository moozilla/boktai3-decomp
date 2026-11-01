#ifndef GUARD_EFFECT_GROUP_H
#define GUARD_EFFECT_GROUP_H

#include "contact_position.h"

struct EffectGroup;
s32 sub_08014634(s32 count, s32 options, const struct ContactPosition *position,
                 const struct ContactPosition *velocity, const struct ContactPosition *spread,
                 s32 lifetime, s32 lifetimeMask);
s32 EffectGroup_SpawnPart(struct EffectGroup *, s32 count, const struct ContactPosition *position,
                          const struct ContactPosition *velocity,
                          const struct ContactPosition *spread, s32 lifetime, s32 lifetimeMask);

#endif
