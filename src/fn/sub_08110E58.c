#include "global.h"
void sub_08110E58(s32 id, u32 a, u32 b, u32 c, u32 d, u32 e)
{
    a <<= 14; b <<= 8; c <<= 7; d <<= 6; e <<= 2;
    switch (id) {
    case 0:
        *(vu16 *)0x04000008 = e | (d | (a | b | c));
        break;
    case 1:
        *(vu16 *)0x0400000A = a | b | c | d | e | 1;
        break;
    case 2:
        *(vu16 *)0x0400000C = a | b | c | d | e | 2;
        break;
    case 3:
        *(vu16 *)0x0400000E = a | b | c | d | e | 3;
        break;
    }
}
