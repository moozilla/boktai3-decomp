#include "global.h"
extern u8 *gGameState;
u32 GameState_HasEquipment(u32 i)
{
    return *(u32 *)(gGameState + 0x4c) & (1 << i);
}
