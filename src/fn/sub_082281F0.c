// COMPILER: old_agbcc
#include "global.h"
/* SaveData_GetPoints: Returns the saved value paired with the clamped points setter; points interpretation is tentative */
#define SaveData_GetPoints sub_082281F0

extern u8 *gUnk_030053F8;

u32 sub_082281F0(void)
{
    return *(u32 *)(gUnk_030053F8 + 12);
}
