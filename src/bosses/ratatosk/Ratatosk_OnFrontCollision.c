#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

void Ratatosk_OnFrontCollision(u32 a, u32 b, struct Ratatosk *owner)
{
    owner->unknownCC0 = 1;
}
