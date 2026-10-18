#include "global.h"
#define ActorReaction_CommitRequestedState  Hresvelgr_ApplyRequestedAction
#define ActorReaction_ResetFlagsAndDispatch Hresvelgr_UpdateBehaviorMode
#define gActorUpdatePaused                  gUnk_0300523C
#define gActorUpdateLatch                   gUnk_02000550
struct ActorReaction {
    u8 unknown0[0xAB];
    u8 state;
    u8 unknownAC[3];
    u8 callbackBlocked;
    u8 unknownB0[0x34];
    u32 ticks;
};
extern u32 gUnk_0300523C;
extern u16 gUnk_02000550;
void sub_081A26D8(struct ActorReaction *);
void Hresvelgr_UpdateBehaviorMode(struct ActorReaction *);
u32 Hresvelgr_ApplyRequestedAction(struct ActorReaction *);
void ActorReaction_UpdateCallbackGate(struct ActorReaction *);
void sub_081A2858(struct ActorReaction *);
void Hresvelgr_CopyUpdateTable(struct ActorReaction *);
u32 ActorReaction_Tick(struct ActorReaction *actor)
{
    if (gActorUpdatePaused) {
        gActorUpdateLatch = 0;
        if (actor->state == 13)
            actor->callbackBlocked = 1;
    } else {
        sub_081A26D8(actor);
        ActorReaction_ResetFlagsAndDispatch(actor);
        ActorReaction_CommitRequestedState(actor);
        ActorReaction_UpdateCallbackGate(actor);
        sub_081A2858(actor);
        Hresvelgr_CopyUpdateTable(actor);
        actor->ticks++;
    }
    return 0;
}
