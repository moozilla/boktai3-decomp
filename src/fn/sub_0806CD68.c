#include "scene_steering_actor.h"
static inline u8 Used(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return 1;
    return 0;
}
void SceneSteeringActor_RequestRemovalAll(void)
{
    struct SceneSteeringActorPool *pool = gSceneSteeringActorPool;
    u32 initial = (u32)pool + 0x3C;
    struct SceneSteeringActor *slot;
    s32 i;
    if (pool) {
        u32 stride;
        u32 one;
        i = 0;
        one = 1;
        slot = (void *)initial;
        stride = sizeof(*slot);
        do {
            if (Used(&pool->used, one << i) && slot->state != 5) {
                slot->state = 5;
                slot->enter = one;
            }
            slot = (void *)((u32)slot + stride);
            i++;
        } while (i < 8);
    }
}
