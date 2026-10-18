// COMPILER: old_agbcc
#include "global.h"
/* SolarBank_GetBalance: Returns the saved balance also used by SolarBank_ApplyInterestFromScript */
#define SolarBank_GetBalance sub_081BB38C

extern u8 *gUnk_02000710;

s16 sub_081BB38C(void)
{
    return *(s16 *)(gUnk_02000710 + 0x7b0);
}
