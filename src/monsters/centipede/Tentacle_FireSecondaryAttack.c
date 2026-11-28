#include "scene/actor/scene_actor_state.h"
#include "global.h"
#include "scene/actor/scene_actor_placement.h"
#include "scene/collision/scene_actor_collision.h"

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

void Tentacle_FireSecondaryAttack(struct ChaseActor *actor)
{
    if (SceneActorPlacement_IsUnavailable(&actor->base))
        return;
    {
        u32 busy = 0xA027;
        u32 flags = actor->base.motionFlags;
        u16 mask;
        if (busy & flags)
            return;
        mask = 0x100;
        if (flags & mask) {
            struct ContactCollider *collider = &actor->base.colliderB;
            u16 field3C, field40, field44;
            u32 reactionFlags;
            collider->flags &= ~mask;
            field3C = actor->base.attacks.first[1];
            field40 = actor->base.attacks.second[1];
            reactionFlags = actor->base.attacks.flags[1];
            field44 = actor->base.attacks.parameter[1];
            Collider_SetAttackProperties(collider, field3C, field40, 0, reactionFlags, field44);
            return;
        }
        {
            struct ContactCollider *collider = &actor->base.colliderB;
            u16 field3C, field40, field44;
            u32 reactionFlags;
            collider->flags |= mask;
            field3C = actor->base.attacks.first[0];
            field40 = actor->base.attacks.second[0];
            reactionFlags = actor->base.attacks.flags[0];
            field44 = actor->base.attacks.parameter[0];
            Collider_SetAttackProperties(collider, field3C, field40, 0, reactionFlags, field44);
        }
    }
}
