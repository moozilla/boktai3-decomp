#include "global.h"

void Camera_ClearTargetOffset(void);

void Camera_Reset(void)
{
    u8 *s = *(u8 **)0x03005420;
    if (s != 0) {
        u32 z = 0;
        *(u16 *)(s + 0x1c) = 1;
        *(u16 *)(s + 0x68) = z;
        Camera_ClearTargetOffset();
    }
}
