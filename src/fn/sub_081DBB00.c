// COMPILER: old_agbcc
#include "global.h"
#include "menu_parameters.h"
/* ScreenParams2C0_GetSecondaryCount: Returns the second value parsed from the script's n option at parameter offset 0x20 */
#define ScreenParams2C0_GetSecondaryCount sub_081DBB00


u32 sub_081DBB00(void)
{
    return gChoiceMenuParameters.n2;
}
