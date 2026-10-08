#include "global.h"
u8 *sub_08055678(void);
void sub_08055740(void)
{
    u8 *p = sub_08055678();
    if (p) {
        u32 v = *(u32 *)(p + 0x40);
        u32 m = 1;
        v |= m;
        *(u32 *)(p + 0x40) = v;
        if (*(u16 *)(p + 0x9C) != 0)
            *(u16 *)(p + 0x9E) = m;
    }
}
