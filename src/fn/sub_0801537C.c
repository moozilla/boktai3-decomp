#include "global.h"
extern u32 gUnk_02000068;
void sub_08214514(u8 *);
u32 sub_0801537C(u8 *s)
{
    u8 *a = s + 0x60;
    u8 *b = s + 0x24;
    s32 i = 0x1f;
    do {
        if (*b != 0)
            sub_08214514(a);
        a += 0x78;
        b += 0x78;
        i--;
    } while (i >= 0);
    {
        u32 z = 0;
        gUnk_02000068 = z;
    }
    return 0;
}
