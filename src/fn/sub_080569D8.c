#include "global.h"
void sub_08217EAC(u8 *);
u32 sub_080569D8(u8 *p)
{
    s32 i = 0;
    if (i < *(u16 *)(p + 0x1C)) {
        u8 *q = p + 0x20;
        do {
            sub_08217EAC(q);
            q += 0x28;
            i++;
        } while (i < *(u16 *)(p + 0x1C));
    }
    return 0;
}
