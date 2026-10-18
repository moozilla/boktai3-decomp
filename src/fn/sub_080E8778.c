#include "global.h"

void Callback_InvokeActor(void *, u32);
void SceneActorControl_DispatchControl(void *);
void Mimic_UpdateReactionState(void *);

struct S {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
};

s32 Mimic_UpdateBehavior(struct S *s)
{
    Callback_InvokeActor(s, s->tbl[s->idx]);
    SceneActorControl_DispatchControl(s);
    Mimic_UpdateReactionState(s);
    return 1;
}
