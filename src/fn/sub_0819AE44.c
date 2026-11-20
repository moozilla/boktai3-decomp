#include "boss_action_state.h"

void ActionState_Reset(struct BossActionState *p)
{
    p->step = 0;
    p->substate = 0;
    p->state = 0;
    p->stepFrame = 0;
    p->substateFrame = 0;
    p->frame = 0;
}
