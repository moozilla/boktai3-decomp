#include "global.h"

u32 sub_08098788(u8 *);
void Callback_InvokeActor(u8 *, u32);
void SceneActorControl_DispatchControl(u8 *);
void ActorContact_UpdateColliderPositions(u8 *);

u32 Ghost_OnBehaviorUpdate(u8 *p)
{
    u32 i;
    u32 *t;
    sub_08098788(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    Callback_InvokeActor(p, t[i]);
    SceneActorControl_DispatchControl(p);
    ActorContact_UpdateColliderPositions(p);
    return 1;
}
