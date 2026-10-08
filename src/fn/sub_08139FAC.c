#include "global.h"
void sub_08139FAC(u32 *p, u32 v)
{
    *p = v;
    *(u8 *)((u8 *)p + 0xcf) = 1;
}
