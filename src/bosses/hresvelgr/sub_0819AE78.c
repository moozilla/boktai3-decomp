// COMPILER: old_agbcc
#include "bosses/hresvelgr/boss_action_state.h"

/* ActionState_ResetSubstate: Clears substate bytes and two scratch words while preserving the main state and first counter */

void ActionState_ResetSubstate(struct BossActionState *p)
{
    p->step = 0;
    p->substate = 0;
    p->stepFrame = 0;
    p->substateFrame = 0;
}
