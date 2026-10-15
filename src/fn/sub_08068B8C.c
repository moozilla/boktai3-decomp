// MATCHING_CONCESSION: register-pinning
#include "bouncing_object.h"
static inline u8 Occupied(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}
#define BouncingObject_Allocate sub_08068B8C
struct BouncingObject *BouncingObject_Allocate(struct BouncingPool *pool)
{
    s32 i = 0;
    register u32 *used asm("r4") = &pool->used;
    for (; i < 12; i++) {
        if (!Occupied(used, 1u << i)) {
            struct BouncingObject *slot = &pool->slots[i];
            *used |= 1u << i;
            slot->index = i;
            return slot;
        }
    }
    return NULL;
}
