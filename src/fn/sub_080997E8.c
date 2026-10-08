#include "global.h"

extern u8 gUnk_08605DF4[];

void sub_080997E8(u8 *p) {
    *(u8 **)(p + 0x288) = gUnk_08605DF4;
}
