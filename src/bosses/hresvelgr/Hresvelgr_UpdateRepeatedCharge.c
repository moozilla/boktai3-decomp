#include "bosses/hresvelgr/hresvelgr_travel.h"

#include "engine/ram.h"

// confidence=low: repeats charges until terrain or the attempt limit ends them.

s32 Hresvelgr_HasCellOverride(void *);
static inline s32 FacesBoundary(struct HresvelgrTravelActor *actor)
{
    s32 direction = actor->targetHeading;
    if (!actor->side) {
        if (((direction + 32) & 255) <= 64)
            return 1;
    } else {
        if (((direction - 32) & 255) <= 64)
            return 1;
    }
    return 0;
}

void Hresvelgr_UpdateRepeatedCharge(struct HresvelgrTravelActor *actor)
{
    struct HresvelgrTravelActor *self = actor;
    struct BossActionState *action = &actor->behavior;
    if (Hresvelgr_EnterVulnerableBehavior(actor)) {
        actor->chargeCount = 0;
        ActionState_Reset(action);
        ActionState_SetState(action, 0);
    }
    switch (action->state) {
        case 0:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 24);
                action->frame++;
            } else if (self->finished)
                ActionState_AdvanceState(action);
            break;
        case 1:
            if (!action->frame) {
                if (Hresvelgr_HasCellOverride(self)) {
                    ActionState_SetState(action, 3);
                    break;
                }
                Hresvelgr_SetActionAnimation(self, 26);
                actor->chargeCount++;
                action->frame++;
            } else if (self->finished) {
                if (actor->chargeCount > 3)
                    ActionState_SetState(action, 4);
                else if (FacesBoundary(self))
                    ActionState_SetState(action, 1);
                else
                    ActionState_SetState(action, 2);
            }
            break;
        case 2:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 2);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 1);
            break;
        case 3:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 25);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 1);
            break;
        case 4:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 12);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 5);
            break;
        case 5:
            actor->behaviorFinished = 1;
            break;
    }
}
