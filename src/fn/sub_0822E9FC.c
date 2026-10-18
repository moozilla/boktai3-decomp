#include "global.h"
extern u8 *gUnk_02000710;
void SolarFruit_SetInventorySlot(u32 i, u16 v)
{
    *(u16 *)(gUnk_02000710 + i * 2 + 0x828) = v;
}
