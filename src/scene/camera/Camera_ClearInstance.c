#include "global.h"

void Camera_ClearInstance(void)
{
    *(u32 *)0x03005420 = 0;
}
