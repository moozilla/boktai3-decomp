#include "global.h"
extern u8 *gUnk_03005420;
void Camera_PanToActor(void *, s32, s32);

void sub_08226454(void *target, s32 arg1, s32 arg2)
{
    u8 *camera = gUnk_03005420;
    if (camera != NULL) {
        *(u16 *)(camera + 0x1e) = 0;
        Camera_PanToActor(target, arg1, arg2);
    }
}
