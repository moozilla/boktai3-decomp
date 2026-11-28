#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "link/wireless/random.h"

s32 Mod(s32, s32);

struct ContactPosition *Ratatosk_GetRandomMovementPoint(struct Ratatosk *self)
{
    return &self->points[Mod(RANDOM_NEXT(), 8)];
}
