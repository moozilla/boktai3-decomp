// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* ScriptActor_ResetCountersFromScript: zeroes three standalone globals (gPlayerSolarActionPending, gPlayerActionPending, gPlayerCueTimer) with no actor/script lookup; sits in the ScriptActor command cluster so  */
#define ScriptActor_ResetCountersFromScript sub_0815AAB8

void sub_0815AAB8(void)
{
    gPlayerSolarActionPending = 0;
    gPlayerActionPending = 0;
    gPlayerCueTimer = 0;
}
