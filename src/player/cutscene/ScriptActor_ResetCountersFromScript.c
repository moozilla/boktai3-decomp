// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* ScriptActor_ResetCountersFromScript: zeroes three standalone globals (gPlayerSolarActionPending, gPlayerActionPending, gPlayerCueTimer) with no actor/script lookup; sits in the ScriptActor command cluster so  */

void ScriptActor_ResetCountersFromScript(void)
{
    gPlayerSolarActionPending = 0;
    gPlayerActionPending = 0;
    gPlayerCueTimer = 0;
}
