// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"
void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorView *);
static inline u8 Unavailable(struct SceneActorRegistryResetView *actor)
{
    register u32 mask asm("r1");
    u32 offset;
    if (actor) {
        mask = 0x40;
        offset = 0x2D4;
        asm("" : "+r"(mask), "+r"(offset));
        if (*(u16 *)((u8 *)actor + offset) & mask)
            return 1;
        mask = 0x20000;
        offset -= 4;
        {
            u32 value = *(u32 *)((u8 *)actor + offset);
            if (value & mask)
                return 1;
        }
        if (actor->health <= 0)
            return 1;
    }
    return 0;
}
void TentacleActor_EnterHitReaction(struct SceneActorRegistryResetView *);
void TentacleBody_OnBodyContact(struct SceneActorBehaviorContactView *source,
                                struct SceneActorBehaviorContactView *destination,
                                struct SceneActorRegistryResetView *actor)
{
    if (Unavailable(actor))
        return;
    SceneActorBehavior_ApplyContactReaction(source, destination, (void *)actor);
    TentacleActor_EnterHitReaction(actor);
}
