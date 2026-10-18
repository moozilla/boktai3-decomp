// COMPILER: old_agbcc
#include "global.h"
/* SaveData_GetCounterB: Returns the second saved halfword after the header magic; counter interpretation is tentative */
#define SaveData_GetCounterB sub_08228240

extern u8 *gUnk_030053F8;

u16 sub_08228240(void)
{
    return *(u16 *)(gUnk_030053F8 + 0x16);
}
