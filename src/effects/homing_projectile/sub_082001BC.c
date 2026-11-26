#include "effects/homing_projectile/homing_projectile.h"

void HomingProjectile_SetBehaviorCallback(struct HomingParams *p, HomingBehavior behavior)
{
    p->behavior = behavior;
}
