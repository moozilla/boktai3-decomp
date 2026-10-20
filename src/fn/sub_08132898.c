#include "global.h"
#include "scene_actor_reset.h"
void SceneActorReaction_ApplyStaggerDamage(struct SceneActorRegistryResetView *);
void SceneActorReaction_ApplyCategoryDamage(struct SceneActorRegistryResetView *);
void SceneActorReaction_EndHitCheckDefeat(struct SceneActorRegistryResetView *);
static inline u32 TestFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
u32 SceneActorBehavior_DispatchFlaggedCategory(struct SceneActorRegistryResetView *actor)
{
    if (actor->enabled == 1 && !TestFlags(actor, 2) && (actor->field2D8 & 0x100000)) {
        switch ((s32)actor->category) {
            case 11:
            case 12:
            case 13:
                SceneActorReaction_ApplyCategoryDamage(actor);
                return 1;
            case 9:
                SceneActorReaction_EndHitCheckDefeat(actor);
                break;
            default:
                SceneActorReaction_ApplyStaggerDamage(actor);
                return 1;
        }
    }
    return 0;
}
