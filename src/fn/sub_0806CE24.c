#include "scene_steering_actor.h"
void ScaledShadow_Destroy(void *);
static inline u8 Used(struct SceneSteeringActorPool *pool, u32 bit)
{
    if (pool->used & bit)
        return 1;
    return 0;
}
s32 SceneSteeringActor_DestroyPool(struct SceneSteeringActorPool *input)
{
    struct SceneSteeringActorPool *pool = input;
    u8 *status;
    void *shadow;
    u32 offset;
    u32 shared;
    s32 i = 0;
    status = &pool->slots[0].shadowStatus;
    shadow = pool->slots[0].shadow;
    offset = 0;
    do {
        if (Used(pool, 1U << i)) {
            if (!*status) {
                ScaledShadow_Destroy(shadow);
                *status = 255;
            }
            {
                u32 location = offset;
                location += 0x3C;
                Sprite_Remove((void *)((shared = (u32)pool) + location));
            }
        }
        {
            u32 stride = sizeof(pool->slots[0]);
            shared = stride;
            status = (void *)((u32)status + stride);
            shadow = (void *)((u32)shadow + shared);
            offset += shared;
        }
        i++;
    } while (i < 8);
    {
        u32 zero = 0;
        gSceneSteeringActorPool = (void *)zero;
    }
    return 0;
}
