#include "global.h"
#include "engine_globals.h"
void Camera_RequestShake(s32);
s32 Collider_Refresh(u8 *);
void Task_Kill(u8 *);
#define H(o) (*(u16 *)(p + (o)))
void HealthBurstEffect_UpdateFadeOut(u8 *p)
{
    Camera_RequestShake((16 - H(0x34e)) * 2);
    if (H(0x34e) <= 5)
        Collider_Refresh(p + 0x94);
    else if (H(0x34e) == 6)
        gEngineFlags &= ~1;
    H(0x34e)++;
    if (H(0x34e) > 15)
        Task_Kill(p);
}
