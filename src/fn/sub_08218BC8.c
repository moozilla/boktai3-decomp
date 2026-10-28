// COMPILER: old_agbcc
#include "global.h"
#include "text.h"
/* Text_ResetFontResource: Clears the global font resource pointer without freeing it */
#define Text_ResetFontResource sub_08218BC8


void sub_08218BC8(void)
{
    gFontResource = 0;
}
