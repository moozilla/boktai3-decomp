#include "global.h"
extern u8 *gUnk_02000710;
u32 GameState_HasEquipment(u32 i)
{
    return *(u32 *)(gUnk_02000710 + 0x4c) & (1 << i);
}
