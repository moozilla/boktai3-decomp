#include "global.h"
#include "ram.h"
#include "named_object_flags.h"
#include "named_object.h"
u32 TetherActor_CopyPosition(struct ContactPosition *);
void TetherActor_SetPosition(struct ContactPosition *);
static inline void subtractVector(struct ContactPosition *d, struct ContactPosition *a,
                                  struct ContactPosition *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
void NamedObject_MoveAttachedTether(struct NamedObject *object)
{
    struct ContactPosition target, delta;
    struct NamedObjectManager *manager = gNamedActorManager;
    if (manager->tetherName == object->name) {
        TetherActor_CopyPosition(&target);
        subtractVector(&delta, &target, &object->body.position);
        if (!NamedObjectFlags_Has(0x200, &object->flags)) {
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
