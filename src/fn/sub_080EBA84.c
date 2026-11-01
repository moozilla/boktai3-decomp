#include "global.h"
#include "ivy_phase.h"
#include "debug.h"

/* Like Ivy_ResetPhase, but the phase's behaviour byte restarts at 1. */
static inline void Ivy_ResetPhaseActive(u8 *p)
{
    u32 one = 1;
    u8 *state = &p[0x2A8];
    u32 zero = 0;
    *state = zero;
    p[0x2A9] = zero;
    p[0x2AA] = one;
    *(u32 *)(p + 0x2AC) = zero;
    p[0x2B1] = one;
}

/* Count frames until the limit at sub + 0x42, then restart the phase
   (see sub_080F0108 for the empty test of the taken flag). */
void sub_080EBA84(u8 *p)
{
    u8 *sub = p + 0x3D4;
    if (SceneActorControl_TakeFlag(&p[0x2B1])) {
        DEBUG_PRINT();
    }
    if (*(u16 *)(sub + 0x42) <= *(s32 *)(p + 0x2AC))
        Ivy_ResetPhaseActive(p);
    else
        (*(s32 *)(p + 0x2AC))++;
}
