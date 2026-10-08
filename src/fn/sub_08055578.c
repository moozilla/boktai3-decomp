#include "global.h"
extern u32 gUnk_0300523C;
void sub_08054EC0(u8 *);
u32 sub_08055578(u8 *p)
{
    s32 i = 0;
    if (i < p[0x19]) {
        u8 *q = p + 0x1C;
        do {
            if (!(gUnk_0300523C != 0 && q[4] == 0))
                sub_08054EC0(q);
            q += 0xB8;
            i++;
        } while (i < p[0x19]);
    }
    return 0;
}
