#include "global.h"
#include "ram.h"
void Camera_ClearSwayCounter(void)
{
    u8 *p = gActiveCamera;
    if (p)
        *(u16 *)(p + 0x8c) = 0;
}
