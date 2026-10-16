#include "scene_steering_actor.h"
extern void *gUnk_08605B98[];
static inline u8 Used(struct SceneSteeringActorPool *pool, u32 bit)
{
    if (pool->used & bit)
        return 1;
    return 0;
}
s32 SceneSteeringActor_UpdatePool(struct SceneSteeringActorPool *input)
{
    struct SceneSteeringActorPool *pool = input;
    struct SceneSteeringActor *base = pool->slots;
    s32 i;
    if (pool->used) {
        void **table;
        u8 *state;
        struct SceneSteeringActor *slot;
        i = 0;
        table = gUnk_08605B98;
        state = &pool->slots[0].state;
        slot = base;
        do {
            if (Used(pool, 1U << i))
                sub_08249240(pool, slot, table[*state]);
            {
                u32 stride = sizeof(*slot);
                state = (void *)((u32)state + stride);
                slot = (void *)((u32)slot + stride);
            }
            i++;
        } while (i < 8);
    }
    return 0;
}
