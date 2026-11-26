#include "bosses/hresvelgr/boss_action_state.h"

void ActionState_SetState(struct BossActionState *p, u32 state)
{
    p->state = state;
    p->step = 0;
    p->substate = 0;
    p->stepFrame = 0;
    p->substateFrame = 0;
    p->frame = 0;
}
