#include "monsters/centipede/centipede_body.h"
void Centipede_ApplyPendingRadialVelocity(struct TentacleBody *);
u32 Centipede_UpdatePendingVelocity(struct TentacleBody *body)
{
    Centipede_ApplyPendingRadialVelocity(body);
    return 1;
}
