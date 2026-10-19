
#ifdef NONMATCHING
// fuzzy: 82.1%
// foundry draft (worker best 82.1%); what differs is mostly register allocation and instruction order
#define ActorBlink_SelectValue sub_0813DB60
#include "global.h"
/* Tentative: the timer alternates the supplied and stored values every four ticks. */
struct ActorBlinkState {
    u8 unknown0[0xAD2];
    u16 alternateValue, timer;
};
u32 sub_0813DB60(struct ActorBlinkState *s, u32 value)
{
    u16 *timer = &s->timer;
    if (*timer) {
        (*timer)--;
        if ((*timer >> 2) & 1) goto alternate;
    }
    return value;
alternate:
    return s->alternateValue;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0813DB60);
#endif
