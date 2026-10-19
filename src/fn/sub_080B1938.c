#include "global.h"

u8 SceneActor_IsFacingNearbyTarget(u8 *);
void Vorn_BeginAction16(u8 *);
void sub_080B2858(u8 *, u32);
void sub_080B23A0(u8 *, u32);

void Vorn_SelectRecoveryAction(u8 *p)
{
    if ((*(u8 **)(p + 0x3d0))[0x6c0]) {
        if (SceneActor_IsFacingNearbyTarget(p))
            Vorn_BeginAction16(p);
        else
            sub_080B2858(p, 1);
    } else
        sub_080B23A0(p, 1);
}
