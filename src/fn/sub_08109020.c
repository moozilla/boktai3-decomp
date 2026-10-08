#include "global.h"

extern u8 *gUnk_020004B0;

void sub_08109020(void *p, u32 a, u32 b)
{
    u8 *g = gUnk_020004B0;

    if (g) {
        *(void **)(g + 0x80) = p;
        *(u32 *)(g + 0x74) = a;
        *(u32 *)(g + 0x78) = b;
    }
}
