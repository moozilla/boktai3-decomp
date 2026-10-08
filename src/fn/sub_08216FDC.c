#include "global.h"

void sub_08216FDC(u32 a, u32 b, u32 c, u32 d)
{
    *(vu16 *)0x04000048 = (b << 8) | a;
    *(vu16 *)0x0400004A = (d << 8) | c;
}
