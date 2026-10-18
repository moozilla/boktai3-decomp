// COMPILER: old_agbcc
#include "global.h"
/* Solar_IsEnabled: Returns the software flag set by Enable and cleared by Disable */
#define Solar_IsEnabled sub_08243ED8

extern u32 gUnk_030035C0;

u32 sub_08243ED8(void)
{
    return gUnk_030035C0;
}
