#include "global.h"

u32 TentacleActor_UpdatePendingVelocity(u8 *);
void Callback_InvokeActor(u8 *, u32);
void SceneActorControl_DispatchControl(u8 *);
void TentacleActor_UpdateComponents(u8 *);

u32 TentacleActor_OnUpdate(u8 *p)
{
    u32 i;
    u32 *t;
    TentacleActor_UpdatePendingVelocity(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    Callback_InvokeActor(p, t[i]);
    SceneActorControl_DispatchControl(p);
    TentacleActor_UpdateComponents(p);
    return 1;
}
