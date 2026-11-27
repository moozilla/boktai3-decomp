#include "bosses/vanargand/vanargand_parts.h"
#define MovingPlatform_SyncTetherStatus MovingPlatform_UpdateTetherFollowFlags
#define gNamedObjectTetherBounds     gUnk_086064B4
#include "global.h"
#include "engine/ram.h"
#include "objects/moving_platforms/named_object.h"
extern const struct ContactPosition gUnk_086064B4;
u32 TetherActor_CopyPosition(struct ContactPosition *);
void TetherActor_RemoveStatusFlags(u32);
void MovingPlatform_ClearAttachmentFlag(u32, s32);
void MovingPlatform_UpdateTetherFollowFlags(struct NamedObject *);
static inline void addVector(struct ContactPosition *d, struct ContactPosition *a,
                             struct ContactPosition *b)
{
    d->x = a->x + b->x;
    d->y = a->y + b->y;
    d->z = a->z + b->z;
}
void MovingPlatform_UpdateTetherAttachment(struct NamedObject *object)
{
    struct ContactPosition target, delta;
    struct NamedObjectManager *manager;
    TetherActor_CopyPosition(&target);
    target.x = target.x + object->motion.x;
    target.y += object->motion.y;
    target.z = target.z + object->motion.z;
    Vanargand_Difference(&delta, &target, &object->body.position);
    manager = gNamedActorManager;
    if (delta.x <= gUnk_086064B4.x && delta.x >= -gUnk_086064B4.x && delta.y <= gUnk_086064B4.y &&
        delta.y >= -gUnk_086064B4.y && delta.z <= gUnk_086064B4.z && delta.z >= -gUnk_086064B4.z) {
        MovingPlatform_ClearAttachmentFlag(manager->tetherName, 1);
        manager->tetherName = object->name;
        MovingPlatform_UpdateTetherFollowFlags(object);
        MovingPlatform_EnableFlags(16, object);
    } else if (object->name == manager->tetherName) {
        manager->tetherName = 0;
        TetherActor_RemoveStatusFlags(7);
        MovingPlatform_DisableFlags(16, object);
    }
}
