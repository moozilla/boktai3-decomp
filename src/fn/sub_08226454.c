#include "global.h"
#include "ram.h"
void Camera_PanToActor(void *, s32, s32);

void sub_08226454(void *target, s32 arg1, s32 arg2)
{
    u8 *camera = gActiveCamera;
    if (camera != NULL) {
        *(u16 *)(camera + 0x1e) = 0;
        Camera_PanToActor(target, arg1, arg2);
    }
}
