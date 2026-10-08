#include "global.h"
extern const u32 gUnk_08605F50[];
void sub_080A88CC(u8 *p)
{
    *(const u32 **)(p + 0x284) = gUnk_08605F50;
}
