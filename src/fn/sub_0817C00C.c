#include "global.h"
#define ActorDecision_UpdatePendingPhase sub_0817C00C
#define ActorDecision_SetTransition SceneVolume_BeginPhase
struct ActorDecisionPosition {
    u32 a, b;
};
struct ActorDecisionState {
    u8 unknown0[0xAA];
    u8 phase, unknownAB[3], selection, pending, reset;
    u8 unknownB1[0x13];
    u32 timer;
    u8 unknownC8[0x3AF];
    u8 transitionBlocked;
    u8 unknown478[8];
    struct ActorDecisionPosition position;
    u8 unknown488[0x58];
    u8 mode, unknown4E1, phaseFlag, unknown4E3[3];
    u16 timeout;
    u8 transition, savedSelection, unknown4EA, reason;
    u8 unknown4EC[8];
    u16 flags;
    u8 unknown4F6[0x72A];
    struct ActorDecisionPosition savedPosition;
};
s32 SceneVolume_GetActivityState(struct ActorDecisionState *);
void SceneVolume_BeginPhase(struct ActorDecisionState *, s32);
static inline u8 consumeReset(struct ActorDecisionState *p)
{
    if (p->reset) {
        p->reset = 0;
        p->pending = 0;
        return 1;
    }
    return 0;
}
void sub_0817C00C(struct ActorDecisionState *p)
{
    struct ActorDecisionState *s = p;
    if (consumeReset(p)) {
        u32 phase = 2;
        p->phase = phase;
    } else if (p->pending == 1) {
        if (s->transitionBlocked)
            SceneVolume_BeginPhase(p, 1);
        else {
            p->savedSelection = p->selection;
            p->transition = 1;
            p->reason = 0;
        }
    }
    p->timer++;
}
