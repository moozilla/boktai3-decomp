#include "bouncing_object.h"
static inline u8 Occupied(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}
struct BouncingObject *BouncingObject_Allocate(struct BouncingPool *pool)
{
    s32 i = 0;
    u32 *used = &pool->used;
    for (; i < 12; i++) {
        used = &pool->used;
        if (!Occupied(used, 1u << i)) {
            struct BouncingObject *slot = &pool->slots[i];
            *used |= 1u << i;
            slot->index = i;
            return slot;
        }
    }
    return NULL;
}
