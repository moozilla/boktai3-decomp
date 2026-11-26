#include "scene/actor/scene_actor_turn.h"
#include "global.h"
#include "scene/actor/scene_actor_reset.h"
void SceneActorReaction_ApplyStaggerDamage(struct ChaseActor *);
void SceneActorReaction_ApplyCategoryDamage(struct ChaseActor *);
void SceneActorReaction_EndHitCheckDefeat(struct ChaseActor *);
u32 SceneActorBehavior_DispatchFlaggedCategory(struct ChaseActor *actor)
{
    if (actor->base.enabled == 1 && !SceneActorTurn_ActorFlags(actor, 2) &&
        (actor->base.hitAttributes & 0x100000)) {
        switch ((s32)actor->base.category) {
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
