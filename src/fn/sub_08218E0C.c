#include "global.h"

u32 *sub_08218CC4(void);

s32 sub_08218E0C(u32 idx)
{
    u32 *tbl = sub_08218CC4();
    idx -= 0x100;
    if (idx <= 0xFF) {
        u32 *e = (u32 *)((idx << 2) + (u32)tbl);
        u32 v = *e;
        s32 cnt = (v & 0x3FC00000) >> 22;
        if (cnt > 0) {
            cnt--;
            if (cnt <= 0)
                *e = 0;
            else
                *e = (v & 0xC03FFFFF) | (cnt << 22);
            return 1;
        }
    }
    return 0;
}
