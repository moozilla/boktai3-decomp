#include "global.h"
extern const u32 gUnk_08605D14[];
void Slime_SetSecondaryStateTable(u8 *p)
{
    *(const u32 **)(p + 0x288) = gUnk_08605D14;
}
