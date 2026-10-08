#include "global.h"

extern u32 gUnk_02000298;
struct E { u8 pad[0x48]; };
void sub_08030BF8(u8 *);
void sub_08217EAC(u8 *);

u32 sub_081D3374(u8 *p)
{
    u8 *e;
    s32 i;
    sub_08030BF8(p);
    e = p + 0x30;
    for (i = 39; i >= 0; i--, e += 0x48) {
        sub_08217EAC(e + 0x20);
    }
    return gUnk_02000298 = 0;
}
