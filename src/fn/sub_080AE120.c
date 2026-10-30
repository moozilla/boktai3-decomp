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
void SceneActorReaction_BeginSideStep(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct SceneActorSideStepContext *context = actor->context;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 flags;
        if (!context->side) {
            flags = 1;
            actor->body.direction = 64;
            actor->tracking.destination = actor->body.position;
            actor->tracking.destination.x -= 160;
        } else {
            flags = 0;
            actor->body.direction = 64;
            actor->tracking.destination = actor->body.position;
            actor->tracking.destination.x += 160;
        }
        SceneActorAnimation_SelectEntryFlags((void *)actor, 16, flags);
    }
    SideStep_SetFlags((void *)&actor->tracking.target, 32);
    if (frame > context->duration)
        actor->field2B0 = 1;
}
