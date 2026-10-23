#include "global.h"
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

/* Count frames until the limit at sub + 0x42, then restart the phase.
   Testing the taken flag (even with an empty body) ends cse's extended
   block there, so cse does not carry `sub` across it: the ROM keeps sub in
   a register instead of folding sub + 0x42 into p + 0x416. */
void sub_080F0108(u8 *p)
{
    u8 *sub = p + 0x3D4;
    if (SceneActorControl_TakeFlag(&p[0x2B1])) {
    }
    if (*(u16 *)(sub + 0x42) <= *(s32 *)(p + 0x2AC))
        Ivy_ResetPhase(p);
    else
        (*(s32 *)(p + 0x2AC))++;
}
