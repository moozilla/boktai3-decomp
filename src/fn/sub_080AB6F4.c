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

#define Tentacle_FireSecondaryAttack sub_080AB6F4

void Tentacle_FireSecondaryAttack(struct SceneActorRegistryActor *actor)
{
    if (IsUnavailable(actor))
        return;
    {
        u32 busy = 0xA027;
        u32 flags = actor->flags2D4;
        u16 mask;
        if (busy & flags)
            return;
        mask = 0x100;
        if (flags & mask) {
            struct ContactCollider *collider = (struct ContactCollider *)((u8 *)actor + 0x188);
            u16 field3C, field40, field44;
            u32 reactionFlags;
            collider->flags &= ~mask;
            field3C = *(u16 *)((u8 *)actor + 0xB6);
            field40 = *(u16 *)((u8 *)actor + 0xBE);
            reactionFlags = *(u32 *)((u8 *)actor + 0xD0);
            field44 = *(u16 *)((u8 *)actor + 0xC6);
            Collider_SetAttackProperties(collider, field3C, field40, 0, reactionFlags, field44);
            return;
        }
        {
            struct ContactCollider *collider = (struct ContactCollider *)((u8 *)actor + 0x188);
            u16 field3C, field40, field44;
            u32 reactionFlags;
            collider->flags |= mask;
            field3C = *(u16 *)((u8 *)actor + 0xB4);
            field40 = *(u16 *)((u8 *)actor + 0xBC);
            reactionFlags = *(u32 *)((u8 *)actor + 0xCC);
            field44 = *(u16 *)((u8 *)actor + 0xC4);
            Collider_SetAttackProperties(collider, field3C, field40, 0, reactionFlags, field44);
        }
    }
}
