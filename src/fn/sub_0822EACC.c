#include "global.h"
extern u8 *gUnk_02000710;
void SolarFruit_ClearEquippedSlot(u32 i)
{
    *(s16 *)(gUnk_02000710 + i * 2 + 0x90) = -1;
}
