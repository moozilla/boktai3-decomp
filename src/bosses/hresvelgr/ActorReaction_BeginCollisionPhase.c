#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, u32, u32, u32);

void ActorReaction_BeginCollisionPhase(struct HresvelgrTravelActor *actor)
{
    Hresvelgr_SetFlags(actor, 0x20000C);
    actor->flags |= 0x40;
    actor->behaviorFlags |= 4;
    if (actor->changed) {
        actor->changed = 0;
        Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData, 15, 1, 0);
        actor->reactionStage = 6;
    }
}
