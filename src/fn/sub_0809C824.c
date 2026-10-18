#include "global.h"

extern u8 gUnk_08605E34[];

void Lun_InitMotionPreset(u8 *p)
{
    *(u8 **)(p + 0x284) = gUnk_08605E34;
}
