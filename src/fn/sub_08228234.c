// COMPILER: old_agbcc
#include "global.h"
/* SaveData_GetCounterA: Returns the first saved halfword after the header magic; counter interpretation is tentative */
#define SaveData_GetCounterA sub_08228234

extern u8 *gUnk_030053F8;

u16 sub_08228234(void)
{
    return *(u16 *)(gUnk_030053F8 + 20);
}
