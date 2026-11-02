#ifndef GUARD_SCENE_ACTOR_CHARGE_OFFSET_H
#define GUARD_SCENE_ACTOR_CHARGE_OFFSET_H

#include "contact_position.h"

extern const s16 gUnk_086149C4[];

struct ContactSwarm;
s32 ContactSwarm_SpawnCharge(struct ContactSwarm *pool, const struct ContactPosition *position,
                             u32 direction, u32 speed, u32 resourceId, u32 cost, u32 option);

static inline s32 SceneActor_ChargeComponent(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}

static inline void SceneActor_SetChargeOffset(u8 direction, struct ContactPosition *offset,
                                             s32 length)
{
    struct ContactPosition *velocity = offset;
    u32 angle = direction;
    s32 speed = length;
    s32 x = SceneActor_ChargeComponent(gUnk_086149C4[(angle + 64) & 255] * speed);
    velocity->x = x;
    velocity->y = 0;
    velocity->z = SceneActor_ChargeComponent(gUnk_086149C4[angle] * speed);
}

#endif
