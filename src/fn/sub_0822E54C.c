// COMPILER: old_agbcc
#include "global.h"
/* GameState_ClearEquipmentSlot: Marks one of the four equipment slots at offset 0x88 empty */
#define GameState_ClearEquipmentSlot sub_0822E54C

extern u8 *gUnk_02000710;

void sub_0822E54C(s32 i)
{
    *(s16 *)(gUnk_02000710 + i * 2 + 0x88) = -1;
}
