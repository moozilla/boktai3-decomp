#include "global.h"
void TentacleActor_ApplyPendingRadialVelocity(void);
u32 TentacleActor_UpdatePendingVelocity(void)
{
    TentacleActor_ApplyPendingRadialVelocity();
    return 1;
}
