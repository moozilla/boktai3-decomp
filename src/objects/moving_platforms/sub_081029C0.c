#include "objects/moving_platforms/named_object.h"

void TetherActor_AddStatusFlags(u32, u32);
void TetherActor_RemoveStatusFlags(u32);

void MovingPlatform_UpdateTetherFollowFlags(struct NamedObject *object)
{
    if ((u16)(object->state - 3) > 1) {
        TetherActor_AddStatusFlags(7, object->name);
    } else {
        TetherActor_AddStatusFlags(3, object->name);
        TetherActor_RemoveStatusFlags(4);
    }
}
