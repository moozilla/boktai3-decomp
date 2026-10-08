#include "global.h"
void sub_08214514(void *);
extern u32 gUnk_020001E4;
u32 sub_081158FC(u8 *p) {
    u8 *e = p + 0x34;
    s32 i = 0x2f;
    do {
        sub_08214514(e);
        e += 0xF8;
        i--;
    } while (i >= 0);
    {
        u32 z = 0;
        gUnk_020001E4 = z;
    }
    return 0;
}
