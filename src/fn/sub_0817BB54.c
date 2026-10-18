#include "global.h"
#define ActorDecision_UpdateTimedPhase sub_0817BB54
#define ActorDecision_SetTransition SceneVolume_BeginPhase
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
    u8 mode, unknown4E1, phaseFlag, unknown4E3[5], transition, savedSelection, unknown4EA, reason;
    u8 unknown4EC[0x734];
    struct ActorDecisionPosition savedPosition;
};
s32 ActorMovement_CanReachTarget(struct ActorDecisionState *);
s32 SceneVolume_FindNearbyPosition(struct ActorDecisionState *);
s32 SceneVolume_GetActivityState(struct ActorDecisionState *);
s32 SceneVolume_ConsumePendingEvent(struct ActorDecisionState *);
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
void sub_0817BB54(struct ActorDecisionState *p)
{
    struct ActorDecisionState *s = p;
    if (consumeReset(p)) {
        s->mode = 3;
        s->savedPosition = s->position;
        {
            u32 phase = 5;
            p->phase = phase;
        }
        s->phaseFlag = 0;
    }
    if (p->timer == 30) {
        s->mode = 3;
        {
            u32 phase = 4;
            p->phase = phase;
        }
    } else if (ActorMovement_CanReachTarget(s))
        SceneVolume_BeginPhase(p, 1);
    else if (SceneVolume_GetActivityState(p))
        SceneVolume_BeginPhase(p, 2);
    else if (p->timer > 60 && SceneVolume_FindNearbyPosition(s) == 1)
        SceneVolume_BeginPhase(p, 3);
    else if (p->timer > 600 || SceneVolume_ConsumePendingEvent(s)) {
        p->savedSelection = p->selection;
        p->transition = 1;
        p->reason = 0;
    } else if (p->pending) {
        s->mode = 3;
        {
            u32 phase = 4;
            p->phase = phase;
        }
    }
    p->timer++;
}
