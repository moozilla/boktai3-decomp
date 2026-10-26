#include "global.h"
#include "ram.h"
void Camera_SetSway(void *, s32, s32);

void sub_082265D8(void *target, s32 arg1, s32 arg2)
{
    u8 *camera = gActiveCamera;
    if (camera != NULL) {
        *(u16 *)(camera + 0x1e) = 0;
        Camera_SetSway(target, arg1, arg2);
    }
}
