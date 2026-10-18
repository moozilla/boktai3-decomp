#include "global.h"
extern const u32 gUnk_08605F94[];
void Vorn_InitSpecialActionTable(u8 *p)
{
    *(const u32 **)(p + 0x288) = gUnk_08605F94;
}
