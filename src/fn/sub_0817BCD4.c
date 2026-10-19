#include "global.h"
#ifdef NONMATCHING
// fuzzy: 98.1%
// foundry draft (worker best 98.1%); what differs is mostly register allocation and instruction order
#define ActorDecision_UpdatePulsePhase sub_0817BCD4
#define ActorDecision_SetTransition SceneVolume_BeginPhase
#define ActorDecision_AdvancePeriodicCue sub_0817B5E4
struct ActorDecisionPosition { u32 a,b; };
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
s32 sub_0817B5E4(struct ActorDecisionState *,u32,s32,s32,s32);
static inline u8 consumeReset(struct ActorDecisionState *p) {
 if(p->reset) {p->reset=0; p->pending=0; return 1;} return 0;
}
void sub_0817BCD4(struct ActorDecisionState *p) {
 struct ActorDecisionState *s=p;
 s32 result;
 consumeReset(p);
 if(ActorMovement_CanReachTarget((void *)s)) SceneVolume_BeginPhase(p,1);
 else if(SceneVolume_GetActivityState(p)) SceneVolume_BeginPhase(p,2);
 else if(p->timer>60 && SceneVolume_FindNearbyPosition((void *)s)==1) SceneVolume_BeginPhase(p,3);
 else if(p->timer>210) {p->savedSelection=p->selection;p->transition=1;p->reason=0;}
 if(s->unknown4EA==2) result=sub_0817B5E4(p,p->timer,30,30,1);
 else result=sub_0817B5E4(p,p->timer,30,30,0);
 if(result) {u32 phase=6;p->phase=phase;}
 p->timer++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0817BCD4);
#endif
