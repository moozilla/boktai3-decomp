#include "global.h"
#include "engine/ram.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"
s32 Hresvelgr_HasAdjacentOverride(struct HresvelgrTravelActor *);
void OffsetActor_UpdateRepeatedAnimation(struct HresvelgrTravelActor *actor)
{
    struct HresvelgrTravelActor *self = actor;
    struct BossActionState *action = &actor->behavior;
    if (Hresvelgr_EnterVulnerableBehavior(actor)) {
        ActionState_Reset(action);
        gOffsetActorAnimationRepeats = 0;
    }
    switch (action->state) {
        case 0:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 24);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 1);
            break;
        case 1:
            if (!action->frame) {
                if (Hresvelgr_HasAdjacentOverride(self))
                    ActionState_SetState(action, 2);
                else {
                    Hresvelgr_SetActionAnimation(self, 10);
                    action->frame++;
                }
            } else if (self->finished) {
                if (++gOffsetActorAnimationRepeats > 4)
                    ActionState_SetState(action, 3);
                else
                    ActionState_SetState(action, 0);
            }
            break;
        case 2:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 25);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 1);
            break;
        case 3:
            actor->behaviorFinished = 1;
            break;
    }
}
