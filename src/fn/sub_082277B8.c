#include "global.h"
s32 Div(s32, s32);
s32 CameraFollow_ConvertDistanceScale(u32 a, s32 b)
{
    return Div(b, 0x100);
}
