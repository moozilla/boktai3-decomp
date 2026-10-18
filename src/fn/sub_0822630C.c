#include "global.h"

void Camera_SnapTo(u32);

void sub_0822630C(u32 a)
{
    u8 *s = *(u8 **)0x03005420;
    if (s != 0) {
        *(u16 *)(s + 0x1e) = 0;
        Camera_SnapTo(a);
    }
}
