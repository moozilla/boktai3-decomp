#include "global.h"

u32 Crow_UpdatePreAction(u8 *);
void Callback_InvokeActor(u8 *, u32);
void SceneActorControl_DispatchControl(u8 *);
void Crow_UpdatePostAction(u8 *);

u32 Crow_Update(u8 *p)
{
    u32 i;
    u32 *t;
    Crow_UpdatePreAction(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    Callback_InvokeActor(p, t[i]);
    SceneActorControl_DispatchControl(p);
    Crow_UpdatePostAction(p);
    return 1;
}
