#include "global.h"
void sub_081395D4(u32 *p, u32 v)
{
    *p = v;
    *(u8 *)((u8 *)p + 0xcb) = 1;
}
