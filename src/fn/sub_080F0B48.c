#include "global.h"

s32 Callback_InvokeActor(void *, void *);
void SceneActorControl_DispatchControl(void *);
void Ivy_SetAuxiliaryColliderFlag(void *);
void Ivy_UpdateColliderPosition(void *);

struct SUpdate {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
};

s32 Ivy_Update(struct SUpdate *s)
{
    Ivy_SetAuxiliaryColliderFlag(s);
    Callback_InvokeActor(s, s->tbl[s->idx]);
    SceneActorControl_DispatchControl(s);
    Ivy_UpdateColliderPosition(s);
    return 1;
}
