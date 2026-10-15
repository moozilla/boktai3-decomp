// MATCHING_CONCESSION: register-pinning
#include "scene_steering_actor.h"
void sub_08013B74(void *);
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
    s32 i = 0;
    status = &pool->slots[0].shadowStatus;
    shadow = pool->slots[0].shadow;
    offset = 0;
    do {
        if (Used(pool, 1U << i)) {
            if (!*status) {
                sub_08013B74(shadow);
                *status = 255;
            }
            {
                u32 location = offset;
                location += 0x3C;
                Sprite_Remove((void *)((u32)pool + location));
            }
        }
        {
            register u32 stride asm("r0") = sizeof(pool->slots[0]);
            status = (void *)((u32)status + stride);
            shadow = (void *)((u32)shadow + stride);
            offset += stride;
        }
        i++;
    } while (i < 8);
    {
        u32 zero = 0;
        gUnk_02000188 = (void *)zero;
    }
    return 0;
}
