#include "global.h"
#include "scene_actor_reaction.h"
struct SceneActorSideStepContext {
    u8 unknown0[0x6B9];
    u8 side;
    u16 duration;
};
static inline void SideStep_SetFlags(struct SceneActorReactionMotion *motion, u8 mask)
{
    motion->flags |= mask;
}
void SceneActorReaction_BeginSideStep(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorSideStepContext *context = actor->base.context;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 flags;
        if (!context->side) {
            flags = 1;
            actor->base.body.direction = 64;
            actor->base.tracking.destination = actor->base.body.position;
            actor->base.tracking.destination.x -= 160;
        } else {
            flags = 0;
            actor->base.body.direction = 64;
            actor->base.tracking.destination = actor->base.body.position;
            actor->base.tracking.destination.x += 160;
        }
        SceneActorAnimation_SelectEntryFlags((void *)actor, 16, flags);
    }
    SideStep_SetFlags((void *)&actor->base.tracking.target, 32);
    if (frame > context->duration)
        actor->base.field2B0 = 1;
}
