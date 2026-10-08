#include "global.h"

extern u8 gUnk_08605C64[];

void sub_08084E50(u8 *p) {
    *(u8 **)(p + 0x29c) = gUnk_08605C64;
}
