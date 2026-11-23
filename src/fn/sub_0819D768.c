#include "global.h"
#include "hresvelgr_travel.h"

void ActorAction_PlayDescentSequence(struct HresvelgrTravelActor *p)
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
                Hresvelgr_SetActionAnimation(actor, 0x15);
                (action->frame)++;
            } else if (actor->finished)
                ActionState_AdvanceState(action);
            break;
        case 1:
            if (action->frame == 0) {
                Hresvelgr_SetActionAnimation(actor, 0x16);
                (action->frame)++;
            } else if (actor->finished)
                ActionState_AdvanceState(action);
            break;
        case 2:
            if (action->frame == 0) {
                Hresvelgr_SetActionAnimation(actor, 0x13);
                (action->frame)++;
            } else if (actor->finished)
                ActionState_AdvanceState(action);
            break;
        case 3:
            if (action->frame == 0) {
                Hresvelgr_SetActionAnimation(actor, 1);
            }
            (action->frame)++;
            if (actor->position.y <= 0x340)
                ActionState_SetState(action, 4);
            break;
        case 4:
            p->behaviorFinished = 1;
            break;
    }
}
