// COMPILER: old_agbcc
#include "global.h"
/* Player_SetField460FromScript: sets player (slot 0) actor's byte +0x460 to 1; distinct flag from the 45d/45f pair, field purpose unclear so named by offset */
#define Player_SetField460FromScript sub_0815AD60

extern u8 *gUnk_02000580;

s32 sub_0815AD60(void)
{
    gUnk_02000580[0x460] = 1;
    return 0;
}
