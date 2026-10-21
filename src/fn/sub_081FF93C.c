#include "global.h"
u32 HomingProjectile_UpdateDefaultState(u8 *);
u32 HomingProjectile_UpdateDefaultBehavior(u8 *p)
{
    return HomingProjectile_UpdateDefaultState(p);
}
