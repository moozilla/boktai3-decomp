#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

u32 TetherActor_GetRequestStatus(void);
void ActorAction_PlayTetherSequence(struct HresvelgrTravelActor *actor)
{
    struct HresvelgrTravelActor *self = actor;
    struct BossActionState *action = &actor->behavior;
    if (Hresvelgr_EnterVulnerableBehavior(actor)) {
        ActionState_Reset(action);
        ActionState_SetState(action, 0);
    }
    switch (action->state) {
        case 0:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 0xE);
                action->frame++;
            } else if (self->finished)
                ActionState_AdvanceState(action);
            break;
        case 1:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 3);
                action->frame++;
            } else if (self->finished)
                ActionState_AdvanceState(action);
            break;
        case 2:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 0xD);
                action->frame++;
            } else if (self->finished || TetherActor_GetRequestStatus())
                ActionState_SetState(action, 4);
            break;
        case 3:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 0x18);
                action->frame++;
            } else if (self->finished)
                ActionState_AdvanceState(action);
            break;
        case 4:
            actor->behaviorFinished = 1;
            break;
    }
}
