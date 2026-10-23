#ifndef GUARD_IVY_PHASE_H
#define GUARD_IVY_PHASE_H
#include "scene_actor_control_phase.h"

/* Restart the vine's phase: clear the phase bytes and the frame count, and
   raise the restart flag. The address comes before the shared zero (source
   evaluation order). */
static inline void Ivy_ResetPhase(u8 *p)
{
    u8 *state = &p[0x2A8];
    u32 zero = 0;
    *state = zero;
    p[0x2A9] = zero;
    p[0x2AA] = zero;
    *(u32 *)(p + 0x2AC) = zero;
    p[0x2B1] = 1;
}
#endif
