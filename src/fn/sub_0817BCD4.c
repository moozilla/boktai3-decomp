// MATCHING_CONCESSION: register-pinning
#include "global.h"
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
    u8 mode, unknown4E1, phaseFlag, unknown4E3[3];
    u16 timeout;
    u8 transition, savedSelection, unknown4EA, reason;
    u8 unknown4EC[0x734];
    struct ActorDecisionPosition savedPosition;
};
s32 ActorMovement_CanReachTarget(struct ActorDecisionState *);
s32 SceneVolume_FindNearbyPosition(struct ActorDecisionState *);
s32 SceneVolume_GetActivityState(struct ActorDecisionState *);
s32 SceneVolume_ConsumePendingEvent(struct ActorDecisionState *);
void SceneVolume_BeginPhase(struct ActorDecisionState *, s32);
s32 ActorEffect_StepTurn(struct ActorDecisionState *, u32, s32, s32, s32);
static inline u8 consumeReset(struct ActorDecisionState *p)
{
    if (p->reset) {
        p->reset = 0;
        p->pending = 0;
        return 1;
    }
    return 0;
}
void ActorDecision_UpdatePulsePhase(struct ActorDecisionState *p)
{
    s32 result;
    struct ActorDecisionState *s = p;
    asm("" : "+r"(s));
    consumeReset(p);
    if (ActorMovement_CanReachTarget(s))
        SceneVolume_BeginPhase(p, 1);
    else if (SceneVolume_GetActivityState(p))
        SceneVolume_BeginPhase(p, 2);
    else if (p->timer > 60 && SceneVolume_FindNearbyPosition(s) == 1)
        SceneVolume_BeginPhase(p, 3);
    else if (p->timer > 210) {
        p->savedSelection = p->selection;
        p->transition = 1;
        p->reason = 0;
    }
    if (s->unknown4EA == 2)
        result = ActorEffect_StepTurn(p, p->timer, 30, 30, 1);
    else
        result = ActorEffect_StepTurn(p, p->timer, 30, 30, 0);
    if (result) {
        u32 phase = 6;
        p->phase = phase;
    }
    p->timer++;
}
