// COMPILER: old_agbcc
#include "global.h"
/* Player_SetField45DFromScript: sets player (slot 0) actor's byte +0x45d to 1; no script/index lookup, field purpose unclear so named by offset */
#define Player_SetField45DFromScript sub_0815AD24

extern u8 *gUnk_02000580;

s32 sub_0815AD24(void)
{
    gUnk_02000580[0x45d] = 1;
    return 0;
}
