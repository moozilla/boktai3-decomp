#include "global.h"
extern u8 *gUnk_02000580[];
s32 sub_08137CA8(u8 *);
void sub_0824923C(u8 *, u32);
s32 sub_08137DE8(u8 *p)
{
    s32 r = sub_08137CA8(p + 0x5c);
    s32 ret;
    if (r >= 0) {
        *(u32 *)(p + 0xdc) = (u32)(gUnk_02000580[r] + 0x24);
        ret = 1;
    } else {
        sub_0824923C(p, *(u32 *)(p + 0x5f8));
        ret = 0;
    }
    return ret;
}
