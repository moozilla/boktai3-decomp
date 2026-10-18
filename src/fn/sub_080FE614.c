#include "global.h"

void Axe_PrepareUpdate(void *);
void Axe_UpdateEffects(void *);
void Callback_InvokeActor(void *, u32);
void SceneActorControl_DispatchControl(void *);

s32 Axe_OnUpdate(void *p)
{
    u8 idx;
    u32 *tbl;

    Axe_PrepareUpdate(p);
    idx = *(u8 *)((u8 *)p + 0x2A8);
    tbl = *(u32 **)((u8 *)p + 0x27C);
    Callback_InvokeActor(p, tbl[idx]);
    SceneActorControl_DispatchControl(p);
    Axe_UpdateEffects(p);
    return 1;
}
