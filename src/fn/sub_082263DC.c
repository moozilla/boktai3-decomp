#include "global.h"
#include "ram.h"
void Camera_TrackActor(void *, s32);

void sub_082263DC(void *target, s32 arg)
{
    u8 *camera = gActiveCamera;
    if (camera != NULL) {
        *(u16 *)(camera + 0x1e) = 0;
        Camera_TrackActor(target, arg);
    }
}
