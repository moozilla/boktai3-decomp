#include "global.h"
void sub_082170FC(s32 pal, s32 idx, u16 color)
{
    ((u16 *)0x5000000)[pal * 16 + idx] = color;
}
