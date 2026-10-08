#include "global.h"
extern u8 *gUnk_020005EC;
void sub_081FE6E0(void);
s32 sub_081FEBCC(s32 i)
{
    u8 *g = gUnk_020005EC;
    s32 r; s32 off; u8 *q;
    if (g != 0) {
        off = i * 0x3ac; q = g + 0x38; r = *(s32 *)(q + off);
    }
    else {
        sub_081FE6E0();
        r = -1;
    }
    return r;
}
