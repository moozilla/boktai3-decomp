#include "global.h"
extern const u32 gUnk_08606010[];
void Vorn_InitActionTable(u8 *p)
{
    *(const u32 **)(p + 0x29c) = gUnk_08606010;
}
