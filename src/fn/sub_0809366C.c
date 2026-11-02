#include "scene_actor_state.h"
#include "global.h"
#include "scene_actor_collision.h"

void Collider_SetAttackProperties(void *, u16, u16, u32, u32, u16);

static inline u32 ActorFlag(struct SceneActorRegistryActor *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}

static inline u8 IsUnavailable(struct SceneActorRegistryActor *actor)
{
    if (actor &&
        (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}

void SceneActorCollision_FireSecondaryAttack(struct SceneActorRegistryActor *actor)
{
    if (IsUnavailable(actor))
        return;
    {
        u16 mask = 0x100;
        if (mask & actor->flags2D4) {
            struct ContactCollider *collider = (struct ContactCollider *)((u8 *)actor + 0x188);
            u16 field3C = ((struct SceneActorRegistryResetView *)actor)->attacks.first[1];
            u16 field40 = ((struct SceneActorRegistryResetView *)actor)->attacks.second[1];
            u32 reactionFlags = ((struct SceneActorRegistryResetView *)actor)->attacks.flags[1];
            u16 field44 = ((struct SceneActorRegistryResetView *)actor)->attacks.parameter[1];
            Collider_SetAttackProperties(collider, field3C, field40, 0, reactionFlags, field44);
            return;
        }
    }
    {
        struct ContactCollider *collider = (struct ContactCollider *)((u8 *)actor + 0x188);
        u16 field3C = ((struct SceneActorRegistryResetView *)actor)->attacks.first[0];
        u16 field40 = ((struct SceneActorRegistryResetView *)actor)->attacks.second[0];
        u32 reactionFlags = ((struct SceneActorRegistryResetView *)actor)->attacks.flags[0];
        u16 field44 = ((struct SceneActorRegistryResetView *)actor)->attacks.parameter[0];
        Collider_SetAttackProperties(collider, field3C, field40, 0, reactionFlags, field44);
    }
}
