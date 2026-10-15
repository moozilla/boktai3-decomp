// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#define SceneActorReaction_UpdateAnimationGate sub_08080008
void SceneActorReaction_UpdateAnimationGate(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group;
    register u16 *field asm("r0");
    register u32 mask asm("r1");
    asm("" : "+r"(group));
    if (actor->field2B2)
        actor->field2B2 = 0;
    if (*(u32 *)((u8 *)actor + 0xF0) <= 0xFFFFF) {
        field = &group->flags414;
        mask = 1;
        mask |= *field;
    } else {
        field = &group->flags414;
        mask = ~1;
        mask &= *field;
    }
    *field = mask;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
}
