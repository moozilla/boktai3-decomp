#include "global.h"

void sub_08013728(u8 *p) {
    *(u32 *)(p + 0xC) |= 1;
    *(u32 *)(p + 0x60) = 0x0801373D;
}
