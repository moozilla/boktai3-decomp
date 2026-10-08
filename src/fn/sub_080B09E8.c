#include "global.h"
extern const u32 gUnk_08605F9C[];
void sub_080B09E8(u8 *p)
{
    *(const u32 **)(p + 0x284) = gUnk_08605F9C;
}
