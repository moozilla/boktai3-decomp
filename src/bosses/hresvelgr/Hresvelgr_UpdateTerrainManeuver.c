#include "bosses/hresvelgr/hresvelgr_travel.h"

#include "engine/ram.h"

// confidence=low: requests movement around adjacent terrain overrides.

s32 Hresvelgr_HasAdjacentOverride(void *);
void Hresvelgr_UpdateTerrainManeuver(struct HresvelgrTravelActor *actor)
{
    struct HresvelgrTravelActor *self = actor;
    struct BossActionState *action = &actor->behavior;
    if (Hresvelgr_EnterVulnerableBehavior(actor))
        ActionState_Reset(action);
    switch (action->state) {
        case 0:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 24);
                action->frame++;
            } else if (self->finished) {
                if (Hresvelgr_HasAdjacentOverride(self))
                    ActionState_SetState(action, 1);
                else
                    ActionState_SetState(action, 2);
            }
            break;
        case 1:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 25);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 2);
            break;
        case 2:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 3);
                action->frame++;
            } else if (self->finished)
                ActionState_AdvanceState(action);
            break;
        case 3:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 4);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 5);
            break;
        case 4:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 6);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 6);
            break;
        case 5:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 5);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 4);
            break;
        case 6:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 7);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 7);
            break;
        case 7:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 12);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 8);
            break;
        case 8:
            actor->behaviorFinished = 1;
            break;
    }
}
