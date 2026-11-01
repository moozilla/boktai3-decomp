#include "global.h"
#include "ivy_phase.h"
#include "debug.h"

/* Count frames; after 30, restart the phase (the count still advances).
   The empty test of the taken flag is what the sibling sub_080F0108 needs
   too: without it cse carries values across and reload picks other
   registers. */
void sub_080EFF6C(u8 *p)
{
    if (SceneActorControl_TakeFlag(&p[0x2B1])) {
        DEBUG_PRINT();
    }
    if (*(s32 *)(p + 0x2AC) > 30)
        Ivy_ResetPhase(p);
    (*(s32 *)(p + 0x2AC))++;
}
