#define gResourceSlotCount gUnk_030051CC
#define gResourceSlotIds   gUnk_030051F0
#include "global.h"
extern u32 gUnk_030051CC;
extern u16 gUnk_030051F0[2];
void ResourceSlots_Reset(void)
{
    s32 i;
    gUnk_030051CC = 0;
    for (i = 0; i < 2; i++)
        gUnk_030051F0[i] |= 0xffff;
}
