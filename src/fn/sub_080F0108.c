#include "global.h"
#include "ivy_phase.h"
#include "debug.h"

/* Count frames until the limit at sub + 0x42, then restart the phase.
   Testing the taken flag (even with an empty body) ends cse's extended
   block there, so cse does not carry `sub` across it: the ROM keeps sub in
   a register instead of folding sub + 0x42 into p + 0x416. */
void sub_080F0108(u8 *p)
{
    u8 *sub = p + 0x3D4;
    if (SceneActorControl_TakeFlag(&p[0x2B1])) {
        DEBUG_PRINT();
    }
    if (*(u16 *)(sub + 0x42) <= *(s32 *)(p + 0x2AC))
        Ivy_ResetPhase(p);
    else
        (*(s32 *)(p + 0x2AC))++;
}
