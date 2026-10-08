#include "global.h"
void sub_08217EAC(void *);
extern u32 gUnk_02000168;
u32 sub_0806A138(u8 *p) {
    u8 *e = p + 0x28;
    s32 i = 0x2f;
    do {
        sub_08217EAC(e);
        e += 0x44;
        i--;
    } while (i >= 0);
    {
        u32 z = 0;
        gUnk_02000168 = z;
    }
    return 0;
}
