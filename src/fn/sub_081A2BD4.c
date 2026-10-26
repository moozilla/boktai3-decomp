#include "global.h"
#include "render.h"
#define ActorReaction_CommitRequestedState  Hresvelgr_ApplyRequestedAction
#define ActorReaction_ResetFlagsAndDispatch Hresvelgr_UpdateBehaviorMode
#define gActorUpdatePaused                  gEnginePauseFlags
#define gActorUpdateLatch                   gUnk_02000550
struct ActorReaction {
    u8 unknown0[0xAB];
    u8 state;
    u8 unknownAC[3];
    u8 callbackBlocked;
    u8 unknownB0[0x34];
    u32 ticks;
};
extern u16 gUnk_02000550;
void Hresvelgr_UpdateAnimationAndTargets(struct ActorReaction *);
void Hresvelgr_UpdateBehaviorMode(struct ActorReaction *);
u32 Hresvelgr_ApplyRequestedAction(struct ActorReaction *);
void ActorReaction_UpdateCallbackGate(struct ActorReaction *);
void Hresvelgr_UpdateCollisionPresentation(struct ActorReaction *);
void Hresvelgr_CopyUpdateTable(struct ActorReaction *);
u32 ActorReaction_Tick(struct ActorReaction *actor)
{
    if (gActorUpdatePaused) {
        gActorUpdateLatch = 0;
        if (actor->state == 13)
            actor->callbackBlocked = 1;
    } else {
        Hresvelgr_UpdateAnimationAndTargets(actor);
        ActorReaction_ResetFlagsAndDispatch(actor);
        ActorReaction_CommitRequestedState(actor);
        ActorReaction_UpdateCallbackGate(actor);
        Hresvelgr_UpdateCollisionPresentation(actor);
        Hresvelgr_CopyUpdateTable(actor);
        actor->ticks++;
    }
    return 0;
}
