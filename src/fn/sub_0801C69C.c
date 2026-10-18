#include "global.h"

void ResultGame_SetUpdateCallback(u8 *p, void *fn)
{
    u8 *a;
    u32 zero;
    *(void **)(p + 0x7a8) = fn;
    a = p + 0x784;
    zero = 0;
    *a = 1;
    *(u32 *)(p + 0x788) = zero;
    *(u8 *)(p + 0xaf) = zero;
    *(u16 *)(p + 0xaa) = zero;
    *(u16 *)(p + 0xa8) = zero;
}
