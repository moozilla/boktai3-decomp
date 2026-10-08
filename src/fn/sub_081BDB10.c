#include "global.h"
void sub_081BDB10(u8 *p, u32 v)
{
    *(u32 *)(p + 0x17F0) = v;
    *(u16 *)(p + 0x174E) = 0;
}
