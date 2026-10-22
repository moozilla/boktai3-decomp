#include "global.h"
extern u8 *gUnk_03005420;
void Camera_TrackActor(void *, s32);

void sub_082263DC(void *target, s32 arg)
{
    u8 *camera = gUnk_03005420;
    if (camera != NULL) {
        *(u16 *)(camera + 0x1e) = 0;
        Camera_TrackActor(target, arg);
    }
}
