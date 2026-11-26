#include "global.h"
void Centipede_ApplyPendingRadialVelocity(u8 *);
u32 Centipede_UpdatePendingVelocity(u8 *actor)
{
    Centipede_ApplyPendingRadialVelocity(actor);
    return 1;
}
