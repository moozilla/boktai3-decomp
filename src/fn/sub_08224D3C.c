#include "global.h"
extern u8 gUnk_03001738[];
extern u8 gUnk_03003A14[];
u32 sub_08245398(void *, u32, void *, u32);
void sub_082455B8(u32, void *);
s32 sub_08224D3C(void)
{
    s32 ret;
    if ((u16)sub_08245398(gUnk_03001738, 0xE64, gUnk_03003A14, 1) == 0) {
        sub_082455B8(3, gUnk_03003A14 + 4);
        ret = 0;
    } else
        ret = -1;
    return ret;
}
