#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

u32 TetherActor_GetRequestStatus(void);
void ActorAction_PlayTetherSequence(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *actor = p;
    struct BossActionState *action = &p->behavior;
    if (Hresvelgr_EnterVulnerableBehavior(p)) {
        ActionState_Reset(action);
        ActionState_SetState(action, 0);
    }
    switch (action->state) {
        case 0:
            if (action->frame == 0) {
                Hresvelgr_SetActionAnimation(actor, 0xE);
                (action->frame)++;
            } else if (actor->finished)
                ActionState_AdvanceState(action);
            break;
        case 1:
            if (action->frame == 0) {
                Hresvelgr_SetActionAnimation(actor, 3);
                (action->frame)++;
            } else if (actor->finished)
                ActionState_AdvanceState(action);
            break;
        case 2:
            if (action->frame == 0) {
                Hresvelgr_SetActionAnimation(actor, 0xD);
                (action->frame)++;
            } else if (actor->finished || TetherActor_GetRequestStatus())
                ActionState_SetState(action, 4);
            break;
        case 3:
            if (action->frame == 0) {
                Hresvelgr_SetActionAnimation(actor, 0x18);
                (action->frame)++;
            } else if (actor->finished)
                ActionState_AdvanceState(action);
            break;
        case 4:
            p->behaviorFinished = 1;
            break;
    }
}
