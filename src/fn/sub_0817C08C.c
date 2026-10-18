#include "global.h"
#define ActorDecision_SetTransition SceneVolume_BeginPhase
struct ActorDecisionPosition {
    u32 a, b;
};
struct ActorDecisionState {
    u8 unknown0[0xAA];
    u8 phase, animationPhase, unknownAC[2], selection, pending, reset;
    u8 unknownB1[0x13];
    u32 timer;
    u32 threshold;
    u8 unknownCC[0x3B4];
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
void sub_0817A16C(struct ActorDecisionState *, struct ActorDecisionPosition *);
void ActorDecision_UpdateThresholdPhase(struct ActorDecisionState *p)
{
    struct ActorDecisionState *s = p;
    if (consumeReset(p)) {
        s->mode = 3;
        sub_0817A16C(s, &s->savedPosition);
        {
            u32 phase = 4;
            p->phase = phase;
        }
    } else if (p->timer < 60 && p->threshold > 0x10000) {
        if (p->pending == 1) {
            s->mode = 3;
            sub_0817A16C(s, &s->savedPosition);
            {
                u32 phase = 4;
                p->phase = phase;
            }
        }
    } else if (p->animationPhase == 3) {
        if (p->pending == 1) {
            p->savedSelection = p->selection;
            p->transition = 1;
            p->reason = 0;
        }
    } else if (p->pending == 1) {
        if (s->unknown4EA == 0)
            s->mode = 0;
        else
            s->mode = 1;
        {
            u32 phase = 3;
            p->phase = phase;
        }
    }
    if (SceneVolume_GetActivityState(p) == 2)
        SceneVolume_BeginPhase(p, 2);
    p->timer++;
}
