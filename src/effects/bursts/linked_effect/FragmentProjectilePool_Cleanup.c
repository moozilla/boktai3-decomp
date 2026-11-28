#include "effects/bursts/linked_effect.h"

u32 FragmentProjectilePool_Cleanup(struct LinkedEffectPool *pool)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if (LinkedEffectPool_IsOccupied(pool, i)) {
            Sprite_Remove(&pool->effects[i].sprite);
            Collider_Remove(&pool->effects[i].colliders[1]);
        }
    }
    gActorEffectManager = 0;
    return 0;
}
