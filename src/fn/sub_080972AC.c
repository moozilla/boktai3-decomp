#include "global.h"

extern u8 gUnk_08605DD4[];

void Ghost_InitMotionPreset(u8 *p)
{
    *(u8 **)(p + 0x284) = gUnk_08605DD4;
}
