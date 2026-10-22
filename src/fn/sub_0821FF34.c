#include "global.h"
struct Pair {
    u32 a, b;
};

void sub_0821FF34(u8 *p, struct Pair *first, struct Pair *second, u16 value)
{
    *(struct Pair *)(p + 0xc) = *first;
    *(struct Pair *)(p + 0x14) = *second;
    *(u16 *)(p + 8) = value;
}
