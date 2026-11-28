#include "bosses/hresvelgr/hresvelgr_travel.h"

#include "engine/ram.h"

void Hresvelgr_UpdateAttackCycle(struct HresvelgrTravelActor *actor)
{
    struct HresvelgrTravelActor *self = actor;
    struct BossActionState *action = &actor->behavior;
    if (Hresvelgr_EnterVulnerableBehavior(actor)) {
        actor->recoveryFrames = 0;
        gHresvelgrAttackCount = 0;
        ActionState_Reset(action);
        ActionState_SetState(action, 0);
    }
    switch (action->state) {
        case 0:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 14);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 1);
            break;
        case 1:
            self->behaviorFlags |= 2;
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 15);
                action->frame++;
            } else if (self->finished)
                ActionState_AdvanceState(action);
            if (Hresvelgr_TestFlags(self, 0x4000000))
                ActionState_SetState(action, 3);
            break;
        case 2:
            self->behaviorFlags |= 2;
            self->behaviorFlags |= 4;
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 3);
                action->frame++;
                break;
            } else if (self->finished) {
                ActionState_SetState(action, 4);
                break;
            }
            if (Hresvelgr_TestFlags(self, 0x4000000))
                ActionState_SetState(action, 3);
            break;
        case 3:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 32);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 4);
            break;
        case 4:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 16);
                action->frame++;
                gHresvelgrAttackCount++;
            } else {
                if (Hresvelgr_TestFlags(self, 0x40000000))
                    ActionState_SetState(action, 7);
                else if ((s32)self->flags < 0)
                    ActionState_SetState(action, 6);
                else if (self->finished) {
                    if (Hresvelgr_TestFlags(self, 0x20000000))
                        ActionState_SetState(action, 5);
                    else
                        ActionState_SetState(action, 10);
                }
            }
            break;
        case 5:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 17);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 9);
            break;
        case 6:
            ActionState_SetState(action, 7);
            break;
        case 7:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 35);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 8);
            break;
        case 8:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 24);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 14);
            break;
        case 9:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 18);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 12);
            break;
        case 10:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 18);
                action->frame++;
            } else if (self->finished) {
                if (gHresvelgrAttackCount <= 5)
                    ActionState_SetState(action, 11);
                else
                    ActionState_SetState(action, 12);
            }
            break;
        case 11:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 20);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 4);
            break;
        case 12:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 19);
                action->frame++;
            } else if (self->finished)
                ActionState_SetState(action, 13);
            break;
        case 13:
            if (!action->frame) {
                Hresvelgr_SetActionAnimation(self, 1);
                action->frame++;
            }
            if (self->position.y <= 832)
                ActionState_SetState(action, 14);
            break;
        case 14:
            actor->behaviorFinished = 1;
            break;
    }
}
