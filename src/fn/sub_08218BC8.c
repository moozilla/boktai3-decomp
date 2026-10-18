// COMPILER: old_agbcc
#include "global.h"
/* Text_ResetFontResource: Clears the global font resource pointer without freeing it */
#define Text_ResetFontResource sub_08218BC8

extern u32 gUnk_03001664;

void sub_08218BC8(void)
{
    gUnk_03001664 = 0;
}
