#include "global.h"
extern u8 *gGameState;
void SolarFruit_SetInventorySlot(u32 i, u16 v)
{
    *(u16 *)(gGameState + i * 2 + 0x828) = v;
}
