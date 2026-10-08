#include "global.h"

extern u8 gUnk_08605EB8[];

void sub_080A1348(u8 *p) {
    *(u8 **)(p + 0x28c) = gUnk_08605EB8;
}
