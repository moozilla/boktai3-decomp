#include "global.h"

u32 Lun_UpdateMovement(u8 *);
void sub_0824923C(u8 *, u32);
void SceneActorControl_DispatchControl(u8 *);
void Lun_UpdateTerrainBounce(u8 *);

u32 Lun_OnUpdate(u8 *p)
{
    u32 i;
    u32 *t;
    Lun_UpdateMovement(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    sub_0824923C(p, t[i]);
    SceneActorControl_DispatchControl(p);
    Lun_UpdateTerrainBounce(p);
    return 1;
}
