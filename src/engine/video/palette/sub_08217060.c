#include "global.h"

void Video_SetBlendControl(u32 a, u32 b, u32 c)
{
    *(vu16 *)0x04000050 = b | (c << 8) | (a << 6);
}
