#include "global.h"
#define ActorDecision_UpdateDelayPhase sub_0817BC64
struct ActorDecisionPosition {
    u32 a, b;
};
struct ActorDecisionState {
    u8 unknown0[0xAA];
    u8 phase, unknownAB[3], selection, pending, reset;
    u8 unknownB1[0x13];
    u32 timer;
    u8 unknownC8[0x3B8];
    struct ActorDecisionPosition position;
    u8 unknown488[0x58];
    u8 mode, unknown4E1, phaseFlag, unknown4E3[3];
    u16 timeout;
    u8 transition, savedSelection, unknown4EA, reason;
    u8 unknown4EC[0x734];
    struct ActorDecisionPosition savedPosition;
};
static inline u8 consumeReset(struct ActorDecisionState *p)
{
    if (p->reset) {
        p->reset = 0;
        p->pending = 0;
        return 1;
    }
    return 0;
}
void sub_0817BC64(struct ActorDecisionState *p)
{
    struct ActorDecisionState *s = p;
    if (consumeReset(p)) {
        u32 phase = 5;
        p->phase = phase;
    }
    s->timeout = 600;
    if (p->timer > 30) {
        p->savedSelection = p->selection;
        p->transition = 1;
        p->reason = 0;
    }
    p->timer++;
}
