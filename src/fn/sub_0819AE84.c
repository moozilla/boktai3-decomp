#include "boss_action_state.h"

void sub_0819AE84(struct BossActionState *p, u8 substate)
{
    p->substate = substate;
    p->step = 0;
    p->stepFrame = 0;
    p->substateFrame = 0;
}
