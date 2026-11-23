#include "vanargand_parts.h"
#define gNamedObjectTetherBounds gUnk_086064B4
#include "global.h"
#include "named_object.h"
extern const struct ContactPosition gUnk_086064B4;
u32 TetherActor_CopyPosition(struct ContactPosition *);
void TetherActor_SetPosition(struct ContactPosition *);
void TetherActor_AddStatusFlags(u32, u32);
void NamedObject_ClearAttachmentFlag(u32, s32);
void NamedObject_SnapTetherToPlatform(struct NamedObjectManager *manager,
                                      struct NamedObject *object)
{
    struct ContactPosition target, delta;
    if (object->kind == 2) {
        TetherActor_CopyPosition(&target);
        Vanargand_Difference(&delta, &target, &object->body.position);
        if (delta.x <= gUnk_086064B4.x && delta.x >= -gUnk_086064B4.x &&
            delta.z <= gUnk_086064B4.z && delta.z >= -gUnk_086064B4.z) {
            TetherActor_SetPosition(&object->body.position);
            NamedObject_ClearAttachmentFlag(manager->tetherName, 1);
            manager->tetherName = object->name;
            TetherActor_AddStatusFlags(3, object->name);
            NamedObject_EnableFlags(16, object);
        }
    }
}
