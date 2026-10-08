#include "global.h"

extern void sub_08223350(void);
void sub_08108C20(void);

void sub_08108BF0(u8 *p)
{
    u8 *r4 = p + 0x6E;
    u32 v = *r4;

    if (v != 0) {
        sub_08223350();
        *r4 = 0;
    } else {
        *(void **)(p + 0x7C) = sub_08108C20;
        *(u32 *)(p + 0x50) = v;
        *(u32 *)(p + 0x3C) = v;
        *(u32 *)(p + 0x40) = v;
        *(u32 *)(p + 0x58) = v;
    }
}
