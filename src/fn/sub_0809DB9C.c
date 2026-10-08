#include "global.h"

extern u8 gUnk_08605E70[];

void sub_0809DB9C(u8 *p) {
    *(u8 **)(p + 0x288) = gUnk_08605E70;
}
