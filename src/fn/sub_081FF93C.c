#include "homing_projectile.h"

s32 HomingProjectile_UpdateDefaultBehavior(struct HomingSlot *p, s32 slot)
{
    return HomingProjectile_UpdateDefaultState(p, slot);
}
