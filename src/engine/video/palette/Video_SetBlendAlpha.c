#include "global.h"

void Video_SetBlendAlpha(u32 a, u32 b)
{
    *(vu16 *)0x04000052 = (b << 8) | a;
}
