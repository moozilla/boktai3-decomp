#include "global.h"

void Video_SetBlendBrightness(u32 v)
{
    *(volatile u16 *)0x04000054 = v;
}
