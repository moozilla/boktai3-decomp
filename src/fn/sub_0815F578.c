// COMPILER: old_agbcc
#include "global.h"
/* Player_GetBasePosition: Returns the position block at offset 0x24 for the indexed player */
#define Player_GetBasePosition sub_0815F578

extern u8 *gUnk_02000580[];

u8 *sub_0815F578(s32 i)
{
    return gUnk_02000580[i] + 0x24;
}
