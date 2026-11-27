#include "effects/projectiles/timed_sprite.h"
#include "engine/ram.h"

void Sprite_Remove(void *);

static inline u8 DirectionalProjectile_IsAllocated(u32 *mask, s32 slot)
{
    if (*mask & (1 << slot))
        return TRUE;
    return FALSE;
}

u32 DirectionalProjectile_DestroyPool(struct TimedSpritePool *pool)
{
    s32 i;
    for (i = 0; i < 12; i++) {
        if (DirectionalProjectile_IsAllocated(&pool->allocated, i))
            Sprite_Remove(&pool->sprites[i]);
    }
    gMovingSpritePool = 0;
    return 0;
}
