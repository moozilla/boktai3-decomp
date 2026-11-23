#ifndef GUARD_BOSS_ACTION_STATE_H
#define GUARD_BOSS_ACTION_STATE_H

#include "global.h"

/* Each level has its own frame counter. Changing a level also resets its children. */
struct BossActionState {
    u8 state, substate, step;
    u8 unused3;
    u32 frame, substateFrame, stepFrame;
};

void ActionState_Reset(struct BossActionState *);
void ActionState_SetState(struct BossActionState *, u32 state);
void ActionState_AdvanceState(struct BossActionState *);
void ActionState_AdvanceSubstate(struct BossActionState *);
void ActionState_AdvanceStep(struct BossActionState *);

#endif
