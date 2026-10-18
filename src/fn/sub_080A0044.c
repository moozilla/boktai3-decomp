#include "global.h"

extern u8 gUnk_08605E94[];

void Crow_InitActionTable(u8 *p)
{
    *(u8 **)(p + 0x284) = gUnk_08605E94;
}
