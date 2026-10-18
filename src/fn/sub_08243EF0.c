// COMPILER: old_agbcc
#include "global.h"
/* Solar_GetInterruptCounter: Returns the counter incremented on each active sensor interrupt */
#define Solar_GetInterruptCounter sub_08243EF0

extern u32 gUnk_030035B4;

u32 sub_08243EF0(void)
{
    return gUnk_030035B4;
}
