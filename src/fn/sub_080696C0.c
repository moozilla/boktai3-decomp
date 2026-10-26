#include "bouncing_object.h"
static inline u8 Occupied(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}
s32 BouncingObject_GetNearbyHeading(const struct ContactPosition *input)
{
    const struct ContactPosition *position = input;
    struct BouncingPool *pool = gBouncingPool;
    struct BouncingObject *slots;
    u32 *firstMask;
    u32 *used;
    struct ContactPosition difference;
    struct ContactPosition *delta;
    s32 i;
    if (!pool)
        return -1;
    slots = pool->slots;
    firstMask = &pool->used;
    if (!*firstMask)
        return -1;
    i = 0;
    used = firstMask;
    delta = &difference;
    do {
        if (Occupied(used, 1u << i)) {
            struct BouncingObject *slot;
            {
                u32 offset = (u32)i * sizeof(*slots);
                struct BouncingObject *base = slots;
                slot = (void *)(offset + (u32)base);
            }
            if (slot->index >= 0 && slot->timer && slot->state != 2) {
                s32 x;
                s32 z;
                s32 distance;
                struct ContactPosition *source = &slot->sprite.position;
                delta->x = slot->sprite.position.x - position->x;
                delta->y = source->y - position->y;
                delta->z = source->z - position->z;
                x = delta->x;
                distance = x * x;
                z = delta->z;
                distance += (u32)(z * z);
                if (distance <= 102399)
                    return Math_Atan2(x, z);
            }
        }
        i++;
    } while (i < 12);
    return -1;
}
