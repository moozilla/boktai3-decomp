#include "global.h"

vu16 *sub_082192B4(s32 n)
{
    switch (n) {
    case 0: return (vu16 *)0x04000012;
    case 1: return (vu16 *)0x04000016;
    case 2: return (vu16 *)0x0400001A;
    default: return (vu16 *)0x0400001E;
    }
}
