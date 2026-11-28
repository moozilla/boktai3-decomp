#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void ActorAction_PlayDescentSequence(struct HresvelgrTravelActor *actor)
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
                Hresvelgr_SetActionAnimation(self, 0x15);
                action->frame++;
            } else if (self->finished)
                ActionState_AdvanceState(action);
            break;
        case 1:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 0x16);
                action->frame++;
            } else if (self->finished)
                ActionState_AdvanceState(action);
            break;
        case 2:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 0x13);
                action->frame++;
            } else if (self->finished)
                ActionState_AdvanceState(action);
            break;
        case 3:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 1);
            }
            action->frame++;
            if (self->position.y <= 0x340)
                ActionState_SetState(action, 4);
            break;
        case 4:
            actor->behaviorFinished = 1;
            break;
    }
}
