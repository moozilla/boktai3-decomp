#include "global.h"
extern u8 gUnk_03001738[];
extern u8 gUnk_03003A14[];
u32 rfu_initializeAPI(void *, u32, void *, u32);
void rfu_setTimerInterrupt(u32, void *);
s32 sub_08224D3C(void)
{
    s32 ret;
    if ((u16)rfu_initializeAPI(gUnk_03001738, 0xE64, gUnk_03003A14, 1) == 0) {
        rfu_setTimerInterrupt(3, gUnk_03003A14 + 4);
        ret = 0;
    } else
        ret = -1;
    return ret;
}
