#include "bosses/hresvelgr/boss_action_state.h"

void ActionState_AdvanceSubstate(struct BossActionState *p)
{
    p->substate++;
    p->step = 0;
    p->stepFrame = 0;
    p->substateFrame = 0;
}
