#include "global.h"
extern u8 *gGameState;
void SolarFruit_ClearEquippedSlot(u32 i)
{
    *(s16 *)(gGameState + i * 2 + 0x90) = -1;
}
