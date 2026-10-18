#include "global.h"

void SceneActorControl_TickHiddenResponse(u8 *);
void Callback_InvokeActor(u8 *, u32);
void SceneActorControl_DispatchControl(u8 *);

u32 Bat_OnUpdate(u8 *p)
{
    u32 i;
    u32 *t;
    SceneActorControl_TickHiddenResponse(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    Callback_InvokeActor(p, t[i]);
    SceneActorControl_DispatchControl(p);
    return 1;
}
