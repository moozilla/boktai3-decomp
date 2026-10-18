#include "global.h"

u32 Slime_ProcessPendingStateChange(u8 *);
void Callback_InvokeActor(u8 *, u32);
void SceneActorControl_DispatchControl(u8 *);
void SceneActorMotion_RefreshGroundHeight(u8 *);

u32 Slime_Update(u8 *p)
{
    u32 i;
    u32 *t;
    Slime_ProcessPendingStateChange(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    Callback_InvokeActor(p, t[i]);
    SceneActorControl_DispatchControl(p);
    SceneActorMotion_RefreshGroundHeight(p);
    return 1;
}
