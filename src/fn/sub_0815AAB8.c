// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* ScriptActor_ResetCountersFromScript: zeroes three standalone globals (gUnk_02000570, gPlayerActionPending, gPlayerCueTimer) with no actor/script lookup; sits in the ScriptActor command cluster so  */
#define ScriptActor_ResetCountersFromScript sub_0815AAB8

extern u16 gUnk_02000570;

void sub_0815AAB8(void)
{
    gUnk_02000570 = 0;
    gPlayerActionPending = 0;
    gPlayerCueTimer = 0;
}
