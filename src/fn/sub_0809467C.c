#include "global.h"

u32 Bee_UpdateGroundHeight(u8 *);
void Callback_InvokeActor(u8 *, u32);
void SceneActorControl_DispatchControl(u8 *);
void Bee_UpdateTimersAndContacts(u8 *);

u32 Bee_OnUpdate(u8 *p)
{
    u32 i;
    u32 *t;
    Bee_UpdateGroundHeight(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    Callback_InvokeActor(p, t[i]);
    SceneActorControl_DispatchControl(p);
    Bee_UpdateTimersAndContacts(p);
    return 1;
}
