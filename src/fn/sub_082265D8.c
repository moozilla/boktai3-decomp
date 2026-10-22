#include "global.h"
extern u8 *gUnk_03005420;
void Camera_SetSway(void *, s32, s32);

void sub_082265D8(void *target, s32 arg1, s32 arg2)
{
    u8 *camera = gUnk_03005420;
    if (camera != NULL) {
        *(u16 *)(camera + 0x1e) = 0;
        Camera_SetSway(target, arg1, arg2);
    }
}
