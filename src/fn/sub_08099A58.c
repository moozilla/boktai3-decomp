#include "global.h"

u32 sub_08098788(u8 *);
void sub_0824923C(u8 *, u32);
void SceneActorControl_DispatchControl(u8 *);
void sub_080989FC(u8 *);

u32 Ghost_OnBehaviorUpdate(u8 *p)
{
    u32 i;
    u32 *t;
    sub_08098788(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    sub_0824923C(p, t[i]);
    SceneActorControl_DispatchControl(p);
    sub_080989FC(p);
    return 1;
}
