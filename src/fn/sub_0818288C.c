#include "global.h"
void sub_081826C8(u32, u32, u32);
void Camera_RequestShake(s32);
void sub_0818288C(u32 p)
{
    sub_081826C8(p, 7, 0x5b);
    Camera_RequestShake(3);
}
