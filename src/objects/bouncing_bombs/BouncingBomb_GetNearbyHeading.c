#include "objects/bouncing_bombs/bouncing_object.h"
#include "scene/motion/scene_actor_terrain.h"
static inline u8 Occupied(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}

s32 BouncingBomb_GetNearbyHeading(const struct ContactPosition *position)
{
    struct BouncingPool *pool = gBouncingPool;
    struct BouncingObject *slots;
    struct ContactPosition delta;
    s32 i;

    if (!pool)
        return -1;
    slots = pool->slots;
    if (!pool->used)
        return -1;
    for (i = 0; i < 12; i++) {
        if (Occupied(&pool->used, 1 << i)) {
            if (slots[i].index >= 0 && slots[i].timer && slots[i].state != 2) {
                SceneActorTerrain_Subtract(&delta, &slots[i].sprite.position, position);
                if (delta.x * delta.x + delta.z * delta.z < 102400)
                    return Math_Atan2(delta.x, delta.z);
            }
        }
    }
    return -1;
}
