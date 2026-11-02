#include "scene_actor_state.h"
#include "scene_actor_registry.h"
void SceneActorMovement_ProjectProbe(struct SceneActorRegistryActor *input, u32 xScale, u32 zScale)
{
    struct SceneActorRegistryActor *actor = input;
    u8 *direction;
    const s16 *sine;
    xScale <<= 2;
    zScale <<= 2;
    {
        u16 *source = (void *)&actor->position.x;
        s32 value;
        sine = gUnk_086149C4;
        direction = (u8 *)actor + 0x2b6;
        value = *source + ((s32)((u32)sine[(*direction + 64) & 255] * xScale) >> 12);
        ((struct SceneActorRegistryResetView *)actor)->reactionStart.x = value;
    }
    {
        u16 value = actor->position.y;
        ((struct SceneActorRegistryResetView *)actor)->reactionStart.y = value;
    }
    {
        u16 *source = (void *)&actor->position.z;
        s32 value;
        value = *source + ((s32)((u32)sine[*direction] * zScale) >> 12);
        ((struct SceneActorRegistryResetView *)actor)->reactionStart.z = value;
    }
    {
        struct ContactPosition *destination = (void *)((u8 *)actor + 0x318);
        struct SceneActorRegistryActor *source;
        source = actor;
        *destination = source->position;
    }
}
