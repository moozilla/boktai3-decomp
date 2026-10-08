#include "global.h"
extern u8 *gUnk_020005EC;
void sub_081FE6E0(void);
void sub_081FDAAC(void);
s32 sub_081FEBF8(s32 i)
{
    u8 *g = gUnk_020005EC;
    s32 r; s32 off; u8 *q;
    if (g == 0) {
        sub_081FE6E0();
        return -1;
    }
    r = 0;
    off = i * 0x3ac; q = g + 0x68;
    if (*(void **)(q + off) == (void *)sub_081FDAAC)
        r = 1;
    return r;
}
