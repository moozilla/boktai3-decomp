#include "global.h"

void Callback_InvokeActor(void *, u32);
void SceneActorControl_DispatchControl(void *);
void Octopus_SetAnimationRefreshFlag(void *);
void Octopus_UpdateBodyColliderPosition(void *);

struct S {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
};

s32 Octopus_Update(struct S *s)
{
    Octopus_SetAnimationRefreshFlag(s);
    Callback_InvokeActor(s, s->tbl[s->idx]);
    SceneActorControl_DispatchControl(s);
    Octopus_UpdateBodyColliderPosition(s);
    return 1;
}
