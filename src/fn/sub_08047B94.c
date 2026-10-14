#include "global.h"

void sub_08047B94(u8 *p, u32 a, u32 b)
{
    if (p) {
        *(u32 *)(p + 0x20) = a;
        *(u32 *)(p + 0x24) = b;
    }
}
