#include "global.h"
#include "engine/ram.h"
#include "objects/moving_platforms/named_object_flags.h"
#include "objects/moving_platforms/named_object.h"
#include "bosses/vanargand/vanargand_parts.h"
u32 TetherActor_CopyPosition(struct ContactPosition *);
void TetherActor_SetPosition(struct ContactPosition *);
void MovingPlatform_MoveAttachedTether(struct NamedObject *object)
{
    struct ContactPosition target, delta;
    struct NamedObjectManager *manager = gNamedActorManager;
    if (manager->tetherName == object->name) {
        TetherActor_CopyPosition(&target);
        Vanargand_Difference(&delta, &target, &object->body.position);
        if (!NamedObjectFlags_Has(0x200, &object->flags)) {
            target.x = target.x + object->motion.x;
            target.y += object->motion.y;
            target.z = target.z + object->motion.z;
            TetherActor_SetPosition(&target);
        }
        {
            u32 mask = ~0x200;
            object->flags = mask & object->flags;
        }
    }
}
