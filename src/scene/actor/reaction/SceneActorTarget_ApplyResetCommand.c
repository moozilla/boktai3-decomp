#include "scene/actor/scene_actor_reaction.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/actor/scene_actor_init_actor.h"
#include "scene/motion/scene_actor_chase_shared.h"
static inline u8 IsDown(struct ChaseActor *actor, u16 *motion)
{
    if (!actor)
        return 0;
    if (*motion & 0x40)
        return 1;
    {
        u32 mask = 0x20000;
        if (actor->base.statusFlags & mask)
            return 1;
    }
    if (actor->base.health <= 0)
        return 1;
    return 0;
}
void SceneActorTarget_ApplyResetCommand(struct ChaseActor *actor, const u16 *argument)
{
    u32 mask = 0x800;
    u16 *flags = &actor->base.motionFlags;
    u16 prior = mask & *flags;
    if (prior)
        return;
    SceneActorReaction_EnableMotionFlags(mask, flags);
    switch (*argument) {
        case 0: {
            u32 bit = 1;
            if (!(actor->base.behaviorFlags & bit))
                return;
        }
            SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 3);
            SceneActorRegistry_ResetRouteAndColliders((void *)actor);
            MonsterInit_SetState(actor, 3, 0, 0);
            SceneActorTerrain_InstallControl(actor, actor->base.onHidden, 12);
            break;
        case 1:
            if (IsDown(actor, flags))
                return;
            SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 3);
            SceneActorRegistry_ResetRoute((void *)actor);
            MonsterInit_SetState(actor, 3, 0, 0);
            SceneActorTerrain_InstallControl(actor, actor->base.onHidden, 12);
            break;
        default: {
            u32 bit = 1;
            if (actor->base.behaviorFlags & bit) {
                SceneActorRegistry_RestoreActorDefaults((void *)actor);
                actor->base.colliderA.responseValue = 0;
            }
        } break;
    }
}
