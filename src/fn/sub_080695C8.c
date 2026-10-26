#include "bouncing_object.h"
static inline u8 Occupied(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}
s32 BouncingObject_DestroyPool(struct BouncingPool *input)
{
    struct BouncingPool *pool = input;
    s32 i = 0;
    u32 *used;
    used = &pool->used;
    do {
        if (Occupied(used, 1u << i)) {
            u32 slotOffset = i;
            struct BouncingObject *slot;
            slotOffset *= sizeof(*slot);
            slotOffset += (u32) & ((struct BouncingPool *)0)->slots;
            slot = (void *)((u32)pool + slotOffset);
            Sprite_Remove(slot);
            Shadow_Destroy(slot->registration);
        }
        i++;
    } while (i < 12);
    gBouncingPool = NULL;
    return 0;
}
