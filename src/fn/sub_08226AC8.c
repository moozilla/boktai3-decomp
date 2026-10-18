#include "global.h"
extern u8 *gUnk_03005420;
void Camera_ClearSwayCounter(void)
{
    u8 *p = gUnk_03005420;
    if (p)
        *(u16 *)(p + 0x8c) = 0;
}
