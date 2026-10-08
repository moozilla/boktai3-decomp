#include "global.h"
u8 *sub_08055678(void);
void sub_08055768(void)
{
    u8 *p = sub_08055678();
    if (p) {
        *(u32 *)(p + 0x40) &= -2;
        if (*(u16 *)(p + 0x9C) != 0)
            *(u16 *)(p + 0x9E) = 0;
    }
}
