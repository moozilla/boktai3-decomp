// MATCHING_CONCESSION: register-pinning
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
        asm("" : "+r"(source));
        sine = gUnk_086149C4;
        direction = (u8 *)actor + 0x2b6;
        value = *source + ((s32)((u32)sine[(*direction + 64) & 255] * xScale) >> 12);
        *(s16 *)((u8 *)actor + 0x310) = value;
    }
    {
        register u16 value asm("r1") = actor->position.y;
        *(s16 *)((u8 *)actor + 0x312) = value;
    }
    {
        u16 *source = (void *)&actor->position.z;
        s32 value;
        value = *source + ((s32)((u32)sine[*direction] * zScale) >> 12);
        *(s16 *)((u8 *)actor + 0x314) = value;
    }
    {
        struct ContactPosition *destination = (void *)((u8 *)actor + 0x318);
        struct SceneActorRegistryActor *source;
        source = actor;
        *destination = source->position;
    }
}
