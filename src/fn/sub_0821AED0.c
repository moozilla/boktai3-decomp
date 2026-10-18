// COMPILER: old_agbcc
#include "global.h"
/* Script_GetSectionThreeData: Returns data after the fourth resolved section pointer's header with section purpose tentative */
#define Script_GetSectionThreeData sub_0821AED0

extern u8 *gUnk_02000448[];

u8 *sub_0821AED0(void)
{
    return gUnk_02000448[3] + 4;
}
