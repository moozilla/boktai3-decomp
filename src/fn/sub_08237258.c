#include "global.h"
void sub_08217EAC(void *);
extern u32 gUnk_020004A0;
u32 sub_08237258(u8 *p) {
    u8 *e = p + 0x18;
    s32 i = 0x7;
    do {
        sub_08217EAC(e);
        e += 0x30;
        i--;
    } while (i >= 0);
    {
        u32 z = 0;
        gUnk_020004A0 = z;
    }
    return 0;
}
