#include "scene_steering_actor.h"
static inline u8 Used(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return 1;
    return 0;
}
// confidence=low: gives every waiting (state 3) steering actor a target, lifetime and vertical
// velocity and moves it to state 4.
void sub_0806CCC8(struct ContactPosition *target, s32 lifetime, s32 speed)
{
    struct SceneSteeringActorPool *pool = gSceneSteeringActorPool;
    u32 initial = (u32)pool + 0x3C;
    struct SceneSteeringActor *slot;
    s32 i;
    if (pool) {
        u32 one;
        i = 0;
        one = 1;
        do {
            slot = (struct SceneSteeringActor *)initial + i;
            if (Used(&pool->used, one << i) && slot->state == 3) {
                slot->lifetime = lifetime;
                {
                    s16 *velocityY = &pool->slots[i].velocity.y;
                    *velocityY = speed;
                }
                slot->target = *target;
                slot->state = 4;
                slot->enter = one;
            }
            i++;
        } while (i < 8);
    }
}
