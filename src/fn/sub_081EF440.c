// MATCHING_CONCESSION: register-pinning
#include "global.h"

// Remaining difference: the first two pointer copies use the opposite register order.
// fix.py --allow sp_loop_weight matches it with `do { other->entered = 1; } while (0)` (loop weight
// raises the priority of `other`); kept as a draft until a plain-C spelling does the same.
#ifdef NONMATCHING
struct Slot7Motion {
    u8 pad0[0xAE];
    u8 phase, reset, entered;
    u8 padB1[0xC4 - 0xB1];
    u32 timer;
    u8 padC8[0x2B4 - 0xC8];
    s32 speed;
    u8 pad2B8[0x5B4 - 0x2B8];
    s32 previousPhase;
};
extern void *gUnk_08613DA8[];
s32 Callback_InvokeActor(void *, void *);
struct PhaseState {
    u8 pad[0xAE];
    u8 phase, reset, entered;
    u8 padB1[0x13];
    u32 timer;
};
static inline void BeginPhase(struct PhaseState *p, s32 phase)
{
    p->phase = phase;
    p->timer = 0;
    p->entered = 1;
}
void Slot7_UpdateMotionPhase(struct Slot7Motion *p)
{
    struct PhaseState *other = (struct PhaseState *)p;
    u8 *reset = &p->reset;
    asm("" : "+r"(other));
    if (*reset) {
        BeginPhase((struct PhaseState *)p, 1);
    }
    Callback_InvokeActor(p, gUnk_08613DA8[p->phase]);
    if ((u8)(p->phase - 1) <= 1) {
        if (p->timer > 60 || (p->speed <= 0x1FFF && p->timer > 30)) {
            if (p->previousPhase == 4) {
                BeginPhase((struct PhaseState *)p, 3);
                p->previousPhase = 3;
            } else {
                u32 phase = 4;
                u32 zero = 0;
                ((struct PhaseState *)p)->phase = phase;
                ((struct PhaseState *)p)->timer = zero;
                other->entered = 1;
                p->previousPhase = 4;
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Slot7_UpdateMotionPhase);
#endif
