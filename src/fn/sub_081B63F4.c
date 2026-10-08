#include "global.h"
void sub_081B63F4(u8 *p, u32 v)
{
    if (p) {
        *(u16 *)(p + 0x14A) = v;
        *(u16 *)(p + 0x366) = v;
    }
}
