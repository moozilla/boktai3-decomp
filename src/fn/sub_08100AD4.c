#define gNamedObjectContactBounds gUnk_086064B4
#include "global.h"
#include "contact_actor.h"
#include "ram.h"
struct NamedObjectContactSlot {
    u8 unknown00[0x10C];
    struct ContactPosition position;
    u8 unknown114[0x19C - 0x114];
};
struct NamedObjectContactPool {
    u8 task[0x1C];
    struct NamedObjectContactSlot slots[12];
    u32 activeMask;
};
extern const struct ContactPosition gUnk_086064B4;
static inline u8 IsActive(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}
static inline void Difference(struct ContactPosition *d, const struct ContactPosition *a,
                              const struct ContactPosition *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
struct NamedObjectContactSlot *NamedObject_FindNearPosition(const struct ContactPosition *position)
{
    struct NamedObjectContactPool *pool = gNamedActorManager;
    struct ContactPosition delta;
    s32 i;
    if (!pool)
        return NULL;
    if (!pool->activeMask)
        return NULL;
    goto search;
found:
    return &pool->slots[i];
search: {
    for (i = 0; i < 12; i++) {
        if (IsActive(&pool->activeMask, i)) {
            Difference(&delta, position, &pool->slots[i].position);
            if (delta.x <= gUnk_086064B4.x && delta.x >= -gUnk_086064B4.x &&
                delta.y <= gUnk_086064B4.y && delta.y >= -gUnk_086064B4.y &&
                delta.z <= gUnk_086064B4.z && delta.z >= -gUnk_086064B4.z)
                goto found;
        }
    }
}
    return NULL;
}
