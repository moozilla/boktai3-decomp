#include "global.h"

void Camera_SetMessageCompletionMode(u8 *p)
{
    *(u16 *)(p + 0x1e) = 2;
}
