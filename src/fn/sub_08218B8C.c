#include "global.h"

extern u32 gUnk_03001660;
extern u8 gUnk_03001560[];
void sub_08219EBC(u32, u32, u32);

void sub_08218B8C(u32 v, u32 b)
{
    if (gUnk_03001660 <= 0x100) {
        gUnk_03001560[gUnk_03001660] = v;
        sub_08219EBC(gUnk_03001660 * 32 + 0x02034C00, b, 0x20);
        gUnk_03001660++;
    }
}
