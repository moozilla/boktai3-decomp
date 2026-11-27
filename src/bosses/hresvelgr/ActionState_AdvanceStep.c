#include "bosses/hresvelgr/boss_action_state.h"

void ActionState_AdvanceStep(struct BossActionState *p)
{
    p->step++;
    p->stepFrame = 0;
}
