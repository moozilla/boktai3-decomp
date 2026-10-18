#include "global.h"

extern u8 gUnk_08605E70[];

void Lun_InitReactionTable(u8 *p)
{
    *(u8 **)(p + 0x288) = gUnk_08605E70;
}
