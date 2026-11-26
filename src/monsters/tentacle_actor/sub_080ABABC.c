#include "engine/shared/global.h"
void TentacleActor_ApplyPendingRadialVelocity(u8 *);
u32 TentacleActor_UpdatePendingVelocity(u8 *actor)
{
    TentacleActor_ApplyPendingRadialVelocity(actor);
    return 1;
}
