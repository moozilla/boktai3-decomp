#include "global.h"

void sub_08013440(u8 *p) {
    *(u32 *)(p + 0xC) |= 1;
    *(u32 *)(p + 0x34) = 0x08013455;
}
