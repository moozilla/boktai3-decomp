#include "global.h"
extern const u32 gUnk_08605DE8[];
void Ghost_InitActionTable(u8 *p)
{
    *(const u32 **)(p + 0x28c) = gUnk_08605DE8;
}
