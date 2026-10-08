#include "global.h"
void sub_08033924(u32, u32);
void sub_0822B2F8(u32);
void sub_080379A4(u8 *p)
{
    if (p[0x1a] != 0) {
        p[0x1a] = 0;
        sub_08033924(*(u32 *)(p + 0x4EC), 0);
        sub_0822B2F8(0x192);
    }
    *(u32 *)(p + 0x20) += 1;
}
