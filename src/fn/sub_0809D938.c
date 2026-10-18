#include "global.h"

extern u8 gUnk_08605E68[];

void Lun_InitSecondaryActionTable(u8 *p)
{
    *(u8 **)(p + 0x28c) = gUnk_08605E68;
}
