#include "scene/actor/scene_actor_state.h"
#include "scene/actor/scene_actor_registry.h"
void SceneActorMovement_ProjectProbe(struct ChaseActor *input, u32 xScale, u32 zScale)
{
    struct ChaseActor *actor = input;
    u8 *direction;
    const s16 *sine;
    xScale <<= 2;
    zScale <<= 2;
    {
        u16 *source = (void *)&actor->base.body.position.x;
        s32 value;
        sine = gUnk_086149C4;
        direction = &actor->base.reactionDirection;
        value = *source + ((s32)((u32)sine[(*direction + 64) & 255] * xScale) >> 12);
        actor->base.reactionStart.x = value;
    }
    {
        u16 value = actor->base.body.position.y;
        actor->base.reactionStart.y = value;
    }
    {
        u16 *source = (void *)&actor->base.body.position.z;
        s32 value;
        value = *source + ((s32)((u32)sine[*direction] * zScale) >> 12);
        actor->base.reactionStart.z = value;
    }
    {
        struct ContactPosition *destination = &actor->base.reactionEnd;
        struct ChaseActor *source;
        source = actor;
        *destination = source->base.body.position;
    }
}
