#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"
s32 DirectionGate_IsAngleInModeRange(struct HresvelgrTravelActor *);
void OffsetActor_UpdateFacingWait(struct HresvelgrTravelActor *actor)
{
    struct HresvelgrTravelActor *self = actor;
    struct BossActionState *action = &actor->behavior;
    if (Hresvelgr_EnterVulnerableBehavior(actor))
        ActionState_Reset(action);
    switch (action->state) {
        case 0:
            if (!action->frame)
                Hresvelgr_SetActionAnimation(self, 1);
            if (DirectionGate_IsAngleInModeRange(self)) {
                action->state = 1;
                Hresvelgr_SetActionAnimation(self, 2);
            } else
                action->frame++;
            if (action->frame > 0x100)
                ActionState_SetState(action, 3);
            break;
        case 1:
            if (self->finished)
                ActionState_SetState(action, 0);
            break;
        case 2:
            if (self->finished)
                ActionState_SetState(action, 0);
            break;
        case 3:
            actor->behaviorFinished = 1;
            break;
    }
}
