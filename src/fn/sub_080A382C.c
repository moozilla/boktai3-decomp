#include "global.h"
extern const u32 gUnk_08605EE0[];
extern const u32 gUnk_08605F24[];
void Spider_SetBehaviorTables(u8 *p)
{
    *(const u32 **)(p + 0x284) = gUnk_08605EE0;
    *(const u32 **)(p + 0x294) = gUnk_08605F24;
}
