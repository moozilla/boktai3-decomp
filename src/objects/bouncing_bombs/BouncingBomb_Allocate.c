#include "objects/bouncing_bombs/bouncing_object.h"
static inline u8 Occupied(struct BouncingPool *pool, s32 i)
{
    if (pool->used & (1 << i))
        return TRUE;
    return FALSE;
}

struct BouncingObject *BouncingBomb_Allocate(struct BouncingPool *pool)
{
    s32 i;

    for (i = 0; i < 12; i++) {
        if (!Occupied(pool, i)) {
            struct BouncingObject *slot = &pool->slots[i];
            pool->used |= 1 << i;
            slot->index = i;
            return slot;
        }
    }
    return NULL;
}
