#include "objects/bouncing_bombs/bouncing_object.h"
static inline u8 Occupied(struct BouncingPool *pool, s32 i)
{
    if (pool->used & (1 << i))
        return TRUE;
    return FALSE;
}

s32 BouncingBomb_DestroyPool(struct BouncingPool *pool)
{
    s32 i;

    for (i = 0; i < 12; i++) {
        if (Occupied(pool, i)) {
            struct BouncingObject *slot = &pool->slots[i];
            Sprite_Remove(slot);
            Shadow_Destroy(slot->registration);
        }
    }
    gBouncingPool = NULL;
    return 0;
}
