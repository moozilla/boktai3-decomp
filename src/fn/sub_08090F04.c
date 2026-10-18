#include "global.h"

u32 Slime_ProcessPendingStateChange(u8 *);
void sub_0824923C(u8 *, u32);
void SceneActorControl_DispatchControl(u8 *);
void sub_08090E60(u8 *);

u32 Slime_Update(u8 *p)
{
    u32 i;
    u32 *t;
    Slime_ProcessPendingStateChange(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    sub_0824923C(p, t[i]);
    SceneActorControl_DispatchControl(p);
    sub_08090E60(p);
    return 1;
}
