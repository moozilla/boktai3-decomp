#include "global.h"

void sub_0822B2F8(u32);

void sub_080A0874(u8 *p) {
    u8 *q = *(u8 **)(p + 4) + 0x48;
    if (*(u16 *)(q + 8) == 5 && *(u16 *)(q + 0xe) == 0) sub_0822B2F8(0xf6);
}
