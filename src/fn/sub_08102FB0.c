#include "global.h"
#include "ram.h"
struct NamedObjectVector {
    s16 x, y, z, pad;
};
struct NamedObject {
    u8 pad0[0x58];
    struct NamedObjectVector offset;
    u8 pad60[0x58];
    u16 flags;
    u8 padBA[0x22];
    u32 name;
    u8 padE0[0x2C];
    struct NamedObjectVector position;
};
struct NamedObjectManager {
    u8 pad0[0x1374];
    u32 tetherName;
};
u32 TetherActor_CopyPosition(struct NamedObjectVector *);
void TetherActor_SetPosition(struct NamedObjectVector *);
static inline void subtractVector(struct NamedObjectVector *d, struct NamedObjectVector *a,
                                  struct NamedObjectVector *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
static inline u8 testFlag(u32 mask, u16 *flags)
{
    if (*flags & mask)
        return 1;
    return 0;
}
void NamedObject_MoveAttachedTether(struct NamedObject *object)
{
    struct NamedObjectVector target, delta;
    struct NamedObjectManager *manager = gNamedActorManager;
    if (manager->tetherName == object->name) {
        TetherActor_CopyPosition(&target);
        subtractVector(&delta, &target, &object->position);
        if (!testFlag(0x200, &object->flags)) {
            target.x = target.x + object->offset.x;
            target.y += object->offset.y;
            target.z = target.z + object->offset.z;
            TetherActor_SetPosition(&target);
        }
        {
            u32 mask = ~0x200;
            object->flags = mask & object->flags;
        }
    }
}
