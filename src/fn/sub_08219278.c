#include "global.h"

vu16 *sub_08219278(s32 n)
{
    switch (n) {
    case 0: return (vu16 *)0x04000010;
    case 1: return (vu16 *)0x04000014;
    case 2: return (vu16 *)0x04000018;
    default: return (vu16 *)0x0400001C;
    }
}
